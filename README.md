# 📁-Google-Drive-File-System-C-Console-Application
A C++ console-based simulation of Google Drive with user login, file/folder management, version control, sharing, recycle bin, and notifications. Built without STL using custom data structures like trees, hash tables, stacks, queues, and linked lists.
This is a C++ console-based project that simulates the core functionality of Google Drive using fundamental data structures. Built as a university lab project (BCS-4B, Data Structures), this file system allows user authentication, file management, directory hierarchy, file sharing, version control, and notification handling — all without using the Standard Template Library (STL). The goal was to implement the logic and data storage manually using core structures like hash tables, stacks, queues, and linked lists.

# 🚀 Features
## 👤 User Management
Sign Up/Login with a username and password.

Security Question & Answer for password recovery.

Session tracking with login/logout timestamps.

## 📄 File Management
Create, Read, Update, and Delete files.

Rollback support to restore older versions of files.

Metadata tracking: owner, permissions, creation/read/share timestamps.

Permissions control (r, w, rw).

Versioning via doubly-linked lists (manual version control system).

Deleted File Recovery via a stack-based Recycle Bin.

## 📁 Directory Management
Files and folders are managed using a custom tree structure.

Navigate and build nested directory structures (root/folder/subfolder/...).

View contents of the current directory.

Cleanup orphan files or files deleted by other users.

## 🔁 File Versioning
Each file maintains a linked list of versions, with:

Version number

Content snapshot

Timestamp

Rollback any file to a previous version with a single command.

## ♻️ Recycle Bin
Deleted files are pushed into a stack, allowing:

Restoration of the latest deleted file.

Auto-cleaning all deleted files for the current user.

Owner-specific access to deleted files.

## 🔔 Notifications System
Each user has a personal notification inbox.

Notifications include:

File shared with the user

File accessed by someone else

Welcome/login messages

## 🔗 File Sharing
Share files with another user with specified permissions.

Auto-generates a notification for the recipient with a timestamp.

## 🕓 Recent Files
Queue-based system tracks the 10 most recently accessed files.

Displays the latest opened or updated file for quick access.

## 🧱 Data Structures Used
| Feature               | Data Structure     | Implementation                 |
| --------------------- | ------------------ | ------------------------------ |
| File/Folder hierarchy | Tree               | Custom `TreeNode` class        |
| File versions         | Doubly linked list | `VersionList` class            |
| Deleted files         | Stack              | `DeletedFileStack`             |
| Recent files          | Queue              | `RecentFilesQueue`             |
| User/File metadata    | Hash table         | Custom chaining-based hash map |
| Notifications         | Singly linked list | Per-user notification queue    |

## 🛠 Technical Stack
Language: C++

Platform: Windows Console

No STL containers: All data structures implemented from scratch

Console UI: Text-based interface using Windows.h for color rendering


## 📁 Project Structure
├── Header.h           # All class declarations & utility functions

├── Defination.cpp     # Class/function implementations

├── Main.cpp           # Application entry point

└── README.md          # Project overview and documentation

## 🧪 Use Cases
Academic Projects – A good demonstration of applying core data structure concepts.

Simulated Cloud Storage – Emulates the behavior of services like Google Drive locally.

Interview Practice – Solid example of implementing version control, authentication, and tree structures without libraries.

## 👨‍💻 Author
Malik Kamran Ali

Data Structures Project

