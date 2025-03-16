use std::sync::{Arc, Mutex, Condvar};
use std::thread;
use std::time::Duration;

// Structure representing the tutor and the shared state (waiting students, tutor's state).
struct Office {
    waiting_students: usize,
    tutor_sleeping: bool,
    students_served: usize, // Track how many students have been helped
}

fn main() {
    // The office will hold the shared state for the tutor and students.
    let office = Arc::new((
        Mutex::new(Office {
            waiting_students: 0,
            tutor_sleeping: true,
            students_served: 0, // Initialize with no students served
        }),
        Condvar::new(),
    ));

    // Create a tutor thread.
    let office_clone = Arc::clone(&office);
    let tutor_thread = thread::spawn(move || {
        tutor(office_clone);
    });

    // Create 10 student threads.
    let mut student_threads = vec![];
    for i in 0..10 {
        let office_clone = Arc::clone(&office);
        let student_thread = thread::spawn(move || {
            student(i, office_clone);
        });
        student_threads.push(student_thread);
    }

    // Wait for all student threads to finish.
    for student in student_threads {
        student.join().unwrap();
    }

    // Wait for the tutor thread to finish after all students have been served.
    tutor_thread.join().unwrap();
}

// Function to simulate the tutor's work.
fn tutor(office: Arc<(Mutex<Office>, Condvar)>) {
    loop {
        let (lock, cvar) = &*office;
        let mut office_state = match lock.lock() {
            Ok(office) => office,
            Err(_) => {
                eprintln!("Error: The lock was poisoned.");
                return; // Exit the loop if the lock is poisoned
            }
        };

        // If there are no students waiting, tutor goes to sleep.
        while office_state.waiting_students == 0 && office_state.students_served < 10 {
            office_state.tutor_sleeping = true;
            println!("Tutor is sleeping...");
            office_state = cvar.wait(office_state).unwrap();
        }

        // If all students have been served, stop the tutor thread.
        if office_state.students_served == 10 {
            println!("Tutor is done for the day.");
            break; // Exit the tutor loop once all students are served
        }

        // Tutor helps a student.
        if office_state.waiting_students > 0 {
            office_state.waiting_students -= 1;
        }
        office_state.tutor_sleeping = false;
        office_state.students_served += 1;
        println!("Tutor is helping a student...");

        // Simulate helping a student for some time.
        thread::sleep(Duration::from_secs(2));

        println!("Tutor has finished helping a student.");
    }
}

// Function to simulate a student's behavior.
fn student(id: usize, office: Arc<(Mutex<Office>, Condvar)>) {
    loop {
        let (lock, cvar) = &*office;
        let mut office_state = match lock.lock() {
            Ok(office) => office,
            Err(_) => {
                eprintln!("Error: The lock was poisoned.");
                return; // Exit the loop if the lock is poisoned
            }
        };

        // If the tutor is sleeping, wake up the tutor.
        if office_state.tutor_sleeping {
            println!("Student {} is waking up the tutor.", id);
            office_state.tutor_sleeping = false;
            cvar.notify_one();
        }

        // If the tutor is helping, try to sit in the hallway if space is available.
        if !office_state.tutor_sleeping && office_state.waiting_students < 3 {
            office_state.waiting_students += 1;
            println!("Student {} is waiting in the hallway.", id);

            // Release the lock while the student waits for the tutor.
            drop(office_state);

            // Simulate waiting for a while before seeking help again.
            thread::sleep(Duration::from_secs(3));

            // Lock again and decrement the number of waiting students after getting help.
            let mut office_state = match lock.lock() {
                Ok(office) => office,
                Err(_) => {
                    eprintln!("Error: The lock was poisoned.");
                    return; // Exit the loop if the lock is poisoned
                }
            };

            if office_state.waiting_students > 0 {
                office_state.waiting_students -= 1;
            }
            // Simulate the student receiving help.
            println!("Student {} is getting help from the tutor.", id);

            // Simulate the student taking some time to get help.
            thread::sleep(Duration::from_secs(2));
        } else {
            println!("Student {} will try again later, no chairs available.", id);
            drop(office_state);

            // Wait a bit before trying again if no space in the hallway.
            thread::sleep(Duration::from_secs(2));
        }

        // Terminate the student thread once all students have been helped
        let office_state = lock.lock().unwrap();
        if office_state.students_served == 10 {
            println!("Student {} is done for the day.", id);
            break;
        }
    }
}
