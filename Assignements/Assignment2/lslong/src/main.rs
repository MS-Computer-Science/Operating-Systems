use std::fs;
use chrono::{DateTime, Local};
use std::os::unix::fs::{MetadataExt, PermissionsExt};
use users::{get_user_by_uid, get_group_by_gid};

fn main() -> std::io::Result<()> {
    let current_dir = ".";
    let mut files = Vec::new();

    // Add "." (current directory)
    if let Ok(metadata) = fs::metadata(".") {
        files.push((".".into(), metadata));
    }

    // Add ".." (parent directory)
    if let Ok(metadata) = fs::metadata("..") {
        files.push(("..".into(), metadata));
    }

    // Read all other entries including hidden files
    for entry in fs::read_dir(current_dir)? {
        if let Ok(entry) = entry {
            let file_name = entry.file_name();
            let name_str = file_name.to_string_lossy().to_string();
            // Skip "." and ".." as we've already added them
            if name_str != "." && name_str != ".." {
                if let Ok(metadata) = entry.metadata() {
                    files.push((name_str, metadata));
                }
            }
        }
    }

    // Sort entries (. and .. first, then directories, then files)
    files.sort_by(|a, b| {
        let a_name = &a.0;
        let b_name = &b.0;
        let a_metadata = &a.1;
        let b_metadata = &b.1;

        // Special handling for . and ..
        if a_name == "." {
            std::cmp::Ordering::Less
        } else if b_name == "." {
            std::cmp::Ordering::Greater
        } else if a_name == ".." {
            std::cmp::Ordering::Less
        } else if b_name == ".." {
            std::cmp::Ordering::Greater
        } else {
            // Then sort by directory status
            let a_is_dir = a_metadata.is_dir();
            let b_is_dir = b_metadata.is_dir();
            
            if a_is_dir && !b_is_dir {
                std::cmp::Ordering::Less
            } else if !a_is_dir && b_is_dir {
                std::cmp::Ordering::Greater
            } else {
                // Finally sort by name
                a_name.cmp(b_name)
            }
        }
    });

    // Print each entry
    for (name, metadata) in files {
        let file_type = if metadata.is_dir() { "d" } 
                       else if metadata.file_type().is_symlink() { "l" }
                       else { "-" };

        // Get permissions (Unix-style representation)
        let mode = metadata.permissions().mode();
        let permissions = format!("{}{}{}{}{}{}{}{}{}", 
            if mode & 0o400 != 0 { "r" } else { "-" },
            if mode & 0o200 != 0 { "w" } else { "-" },
            if mode & 0o100 != 0 { "x" } else { "-" },
            if mode & 0o040 != 0 { "r" } else { "-" },
            if mode & 0o020 != 0 { "w" } else { "-" },
            if mode & 0o010 != 0 { "x" } else { "-" },
            if mode & 0o004 != 0 { "r" } else { "-" },
            if mode & 0o002 != 0 { "w" } else { "-" },
            if mode & 0o001 != 0 { "x" } else { "-" }
        );

        // Get owner and group names
        let uid = metadata.uid();
        let gid = metadata.gid();
        let owner = get_user_by_uid(uid)
            .map(|user| user.name().to_string_lossy().into_owned())
            .unwrap_or_else(|| uid.to_string());
        let group = get_group_by_gid(gid)
            .map(|group| group.name().to_string_lossy().into_owned())
            .unwrap_or_else(|| gid.to_string());

        // Get number of links
        let nlink = metadata.nlink();

        // Get file size
        let size = metadata.len();

        // Get modification time
        let modified: DateTime<Local> = DateTime::from(metadata.modified()?);
        let mod_time = modified.format("%b %d %H:%M").to_string();

        // Print the entry
        println!("{}{} {:>3} {:>8} {:>8} {:>8} {} {}", 
            file_type,
            permissions,
            nlink,
            owner,
            group,
            size,
            mod_time,
            name
        );
    }

    Ok(())
}