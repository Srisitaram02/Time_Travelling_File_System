# Time-Travelling File System (C++)

An in-memory, command-line file system that supports versioning, snapshots, rollbacks, and historical inspection, inspired by modern version control systems.  
The system is implemented entirely from scratch in C++, including custom data structures for trees, hash maps, and heaps.

---

## Features

- Create and manage multiple files
- Maintain a tree-based version history per file
- Support immutable snapshots with timestamps and messages
- Roll back to parent versions or specific version IDs
- View snapshot history along the active branch
- List recently modified files
- Identify files with the largest version trees
- Fast lookups using custom hash maps
- Efficient analytics using custom max-heaps

---

## System Architecture

### File Model
Each file maintains its own version history as a tree:
- Every node represents a version
- The active version pointer tracks the current state
- Version IDs are assigned sequentially per file

### Core Data Structures (Custom Implementations)
- **Tree**: Represents version history of a file
- **Hash Map**:
  - Maps version IDs to version nodes
  - Maps filenames to file objects
- **Max Heap**:
  - Tracks most recently modified files
  - Tracks files with the largest version trees

No STL implementations of these data structures are used.

---

## Version Semantics

- Only snapshotted versions are immutable
- Non-snapshotted versions may be edited in place
- Snapshot versions store a message and timestamp
- Rollbacks do not delete versions; they only move the active pointer

---

## Command Reference

### Core File Operations

#### CREATE `<filename>`
Creates a new file with:
- Root version (ID 0)
- Empty content
- Initial snapshot

---

#### READ `<filename>`
Displays the content of the currently active version.

---

#### INSERT `<filename>` `<content>`
Appends content to the file.
- Creates a new version if the active version is a snapshot
- Otherwise modifies the active version in place

---

#### UPDATE `<filename>` `<content>`
Replaces the entire content of the file.
- Follows the same versioning rules as INSERT

---

#### SNAPSHOT `<filename>` `<message>`
Marks the active version as immutable and records:
- Snapshot message
- Timestamp

---

#### ROLLBACK `<filename>` `[versionID]`
- Without `versionID`: rolls back to the parent version
- With `versionID`: rolls back directly to the specified version

---

#### HISTORY `<filename>`
Lists all snapshot versions on the path from the active version to the root, displayed chronologically with:
- Version ID
- Timestamp
- Snapshot message

---

### System-Wide Analytics

#### RECENT_FILES `<num>`
Lists files in descending order of last modification time (up to `<num>` entries).

---

#### BIGGEST_TREES `<num>`
Lists files in descending order of total version count (up to `<num>` entries).

---

#### EXIT
Terminates the program.

---

## How to Compile and Run

### Compile

```bash
g++ -std=c++17 main.cpp -o filesystem

### Run

./filesystem

```bash
g++ -std=c++17 main.cpp -o filesystem
