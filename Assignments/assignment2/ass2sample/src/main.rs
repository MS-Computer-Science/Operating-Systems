use std::fs;
use chrono::{DateTime, Local};
use std::os::unix::fs::{MetadataExt, PermissionsExt};
use users::{get_user_by_uid, get_group_by_gid};

struct FileInfo {
    name: String,
    is_dir: bool,
    permissions: u32,
    owner: String,
    group: String,
    size: u64,
    modified: String,
}

fn get_file_info(path: &str) -> std::io::Result<FileInfo> {
    let metadata = fs::metadata(path)?;
    
    // Get owner and group
    let owner = get_user_by_uid(metadata.uid())
        .map(|u| u.name().to_string_lossy().into_owned())
        .unwrap_or_else(|| metadata.uid().to_string());
    
    let group = get_group_by_gid(metadata.gid())
        .map(|g| g.name().to_string_lossy().into_owned())
        .unwrap_or_else(|| metadata.gid().to_string());

    // Format modification time
    let modified: DateTime<Local> = DateTime::from(metadata.modified()?);
    let formatted_time = modified.format("%b %d %H:%M").to_string();

    Ok(FileInfo {
        name: path.to_string(),
        is_dir: metadata.is_dir(),
        permissions: metadata.permissions().mode(),
        owner,
        group,
        size: metadata.len(),
        modified: formatted_time,
    })
}

fn format_permissions(mode: u32) -> String {
    let chars = ['r', 'w', 'x'];
    let mut result = String::new();
    
    for i in (0..9).rev() {
        result.push(if mode & (1 << i) != 0 {
            chars[2 - (i % 3)]
        } else {
            '-'
        });
    }
    
    result
}

fn main() -> std::io::Result<()> {
    let mut files = Vec::new();
    
    // Add . and .. first
    files.push(get_file_info(".")?);
    files.push(get_file_info("..")?);
    
    // Add all other files
    for entry in fs::read_dir(".")? {
        let entry = entry?;
        let name = entry.file_name().to_string_lossy().to_string();
        if name != "." && name != ".." {
            files.push(get_file_info(&name)?);
        }
    }
    
    // Sort files: directories first, then by name
    files.sort_by(|a, b| {
        if a.name == "." { return std::cmp::Ordering::Less; }
        if b.name == "." { return std::cmp::Ordering::Greater; }
        if a.name == ".." { return std::cmp::Ordering::Less; }
        if b.name == ".." { return std::cmp::Ordering::Greater; }
        
        match (a.is_dir, b.is_dir) {
            (true, false) => std::cmp::Ordering::Less,
            (false, true) => std::cmp::Ordering::Greater,
            _ => a.name.cmp(&b.name),
        }
    });
    
    // Print files
    for file in files {
        let file_type = if file.is_dir { 'd' } else { '-' };
        println!("{}{} {:>8} {:>8} {:>8} {} {}", 
            file_type,
            format_permissions(file.permissions),
            file.owner,
            file.group,
            file.size,
            file.modified,
            file.name
        );
    }
    
    Ok(())
}
