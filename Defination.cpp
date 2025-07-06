#include "Header.h"
// Utility Functions
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

string getCurrentDateTime() {
    time_t now = time(0);
    char buf[80];
    struct tm timeinfo;
    localtime_s(&timeinfo, &now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return string(buf);
}

// User class implementation
User::User() : username(""), password(""), securityQuestion(""), securityAnswer(""), lastLogin(""), lastLogout("") {}

User::User(string u, string p, string q, string a)
    : username(u), password(p), securityQuestion(q), securityAnswer(a), lastLogin(""), lastLogout("") {
}

string User::getUsername() { return username; }
string User::getSecurityQuestion() { return securityQuestion; }
bool User::checkSecurityAnswer(string answer) { return securityAnswer == answer; }
bool User::verifyPassword(string p) { return password == p; }
void User::setLastLogin() { lastLogin = getCurrentDateTime(); }
void User::setLastLogout() { lastLogout = getCurrentDateTime(); }

// FileMetadata class implementation
FileMetadata::FileMetadata() : name(""), path(""), size(0), type(""), dateCreated(""), owner(""), permissions(""),
creationTime(""), lastReadTime(""), lastSharedTime("") {
}

FileMetadata::FileMetadata(string n, string p, string t, string o)
    : name(n), path(p), size(0), type(t), dateCreated(getCurrentDateTime()), owner(o),
    permissions("rw"), creationTime(getCurrentDateTime()), lastReadTime(""), lastSharedTime("") {
}

void FileMetadata::setLastReadTime() { lastReadTime = getCurrentDateTime(); }
void FileMetadata::setLastSharedTime() { lastSharedTime = getCurrentDateTime(); }
string FileMetadata::getLastReadTime() { return lastReadTime; }
string FileMetadata::getLastSharedTime() { return lastSharedTime; }
string FileMetadata::getOwner() { return owner; }
string FileMetadata::getName() { return name; }
string FileMetadata::getPath() { return path; }
string FileMetadata::getType() { return type; }
string FileMetadata::getPermissions() { return permissions; }
string FileMetadata::getCreationTime() { return creationTime; }
void FileMetadata::setPermissions(string p) { permissions = p; }
bool FileMetadata::hasPermission(string username, string requiredPermission) {
    if (owner == username) return true;
    if (permissions.find(requiredPermission) != string::npos) return true;
    return false;
}

// VersionNode class implementation
VersionNode::VersionNode() : versionNumber(0), timestamp(""), content("") {}
VersionNode::VersionNode(int v, string c) : versionNumber(v), timestamp(getCurrentDateTime()), content(c) {}

// TreeNode class implementation
void TreeNode::resizeChildren() {
    capacity *= 2;
    TreeNode** newChildren = new TreeNode * [capacity];
    for (int i = 0; i < childCount; i++) {
        newChildren[i] = children[i];
    }
    delete[] children;
    children = newChildren;
}

TreeNode::TreeNode() : name(""), type("folder"), parent(nullptr), children(new TreeNode* [10]), childCount(0), capacity(10) {}

TreeNode::TreeNode(string n, string t, TreeNode* p) : name(n), type(t), parent(p), children(new TreeNode* [10]), childCount(0), capacity(10) {}

TreeNode::~TreeNode() {
    for (int i = 0; i < childCount; i++) {
        delete children[i];
    }
    delete[] children;
}

void TreeNode::setName(string n) { name = n; }
string TreeNode::getName() { return name; }
string TreeNode::getType() { return type; }
TreeNode* TreeNode::getParent() { return parent; }
int TreeNode::getChildCount() { return childCount; }
TreeNode* TreeNode::getChild(int index) { return (index >= 0 && index < childCount) ? children[index] : nullptr; }
void TreeNode::addChild(TreeNode* child) {
    if (childCount >= capacity) {
        resizeChildren();
    }
    children[childCount++] = child;
}

TreeNode* TreeNode::findChild(string name) {
    for (int i = 0; i < childCount; i++) {
        if (children[i]->getName() == name) {
            return children[i];
        }
    }
    return nullptr;
}

void TreeNode::setChildCount(int count) { childCount = count; }
TreeNode** TreeNode::getChildren() { return children; }

// DeletedFile class implementation
DeletedFile::DeletedFile() : path(""), content(""), deletionTime(""), owner("") {}

DeletedFile::DeletedFile(string p, string c, string o)
    : path(p), content(c), deletionTime(getCurrentDateTime()), owner(o) {
}

string DeletedFile::getPath() const { return path; }
string DeletedFile::getContent() const { return content; }
string DeletedFile::getDeletionTime() const { return deletionTime; }
string DeletedFile::getOwner() const { return owner; }

// Notification class implementation
Notification::Notification(string msg) : message(msg), timestamp(getCurrentDateTime()), read(false) {}

string Notification::getMessage() const { return message; }
string Notification::getTimestamp() { return timestamp; }
bool Notification::isRead() { return read; }
void Notification::markAsRead() { read = true; }

// DeletedFileStack implementation
DeletedFileStack::Node::Node(DeletedFile d) : data(d), next(nullptr) {}

void DeletedFileStack::removeNode(Node* node, Node* prev) {
    if (prev) {
        prev->next = node->next;
    }
    else {
        top = node->next;
    }
    delete node;
    size--;
}

DeletedFileStack::DeletedFileStack() : top(nullptr), size(0) {}

DeletedFileStack::~DeletedFileStack() {
    clear();
}

void DeletedFileStack::push(DeletedFile file) {
    Node* newNode = new Node(file);
    newNode->next = top;
    top = newNode;
    size++;
}

DeletedFile DeletedFileStack::pop(string currentUser) {
    if (!top) return DeletedFile();

    Node* current = top, * prev = nullptr;
    while (current && current->data.getOwner() != currentUser) {
        prev = current;
        current = current->next;
    }

    if (!current) return DeletedFile();

    DeletedFile data = current->data;
    removeNode(current, prev);
    return data;
}

bool DeletedFileStack::isEmpty(string currentUser) const {
    if (currentUser.empty()) return size == 0;

    Node* current = top;
    while (current) {
        if (current->data.getOwner() == currentUser) {
            return false;
        }
        current = current->next;
    }
    return true;
}

int DeletedFileStack::getCount(string currentUser) const {
    int count = 0;
    Node* current = top;
    while (current) {
        if (current->data.getOwner() == currentUser) {
            count++;
        }
        current = current->next;
    }
    return count;
}

void DeletedFileStack::clear(string currentUser) {
    Node* current = top;
    Node* prev = nullptr;

    while (current) {
        if (currentUser.empty() || current->data.getOwner() == currentUser) {
            Node* toDelete = current;
            current = current->next;
            removeNode(toDelete, prev);
        }
        else {
            prev = current;
            current = current->next;
        }
    }
}

void DeletedFileStack::displayAll(string currentUser) const {
    if (isEmpty(currentUser)) {
        cout << "Recycle bin is empty for user: " << currentUser << endl;
        return;
    }

    cout << "Recycle bin contents for user: " << currentUser << endl;
    cout << "---------------------------------" << endl;

    Node* current = top;
    int index = 1;
    while (current) {
        if (current->data.getOwner() == currentUser) {
            cout << index++ << ". " << current->data.getPath()
                << " (deleted: " << current->data.getDeletionTime() << ")" << endl;
        }
        current = current->next;
    }
}

// RecentFilesQueue implementation
RecentFilesQueue::Node::Node(string d) : data(d), next(nullptr) {}

RecentFilesQueue::RecentFilesQueue() : front(nullptr), rear(nullptr), size(0) {}

RecentFilesQueue::~RecentFilesQueue() {
    while (!isEmpty()) {
        dequeue();
    }
}

void RecentFilesQueue::enqueue(string path) {
    if (size >= capacity) {
        dequeue();
    }

    Node* newNode = new Node(path);
    if (!rear) {
        front = rear = newNode;
    }
    else {
        rear->next = newNode;
        rear = newNode;
    }
    size++;
}

string RecentFilesQueue::dequeue() {
    if (!front) return "";

    Node* temp = front;
    string data = temp->data;
    front = front->next;
    if (!front) rear = nullptr;
    delete temp;
    size--;
    return data;
}

string RecentFilesQueue::displayRecent() {
    if (front) return front->data;
    return "No recent files";
}

bool RecentFilesQueue::isEmpty() { return size == 0; }

// VersionList implementation
VersionList::Node::Node(VersionNode d) : data(d), prev(nullptr), next(nullptr) {}

VersionList::VersionList() : head(nullptr), tail(nullptr), size(0) {}

VersionList::~VersionList() {
    Node* current = head;
    while (current) {
        Node* next = current->next;
        delete current;
        current = next;
    }
}

void VersionList::append(VersionNode version) {
    Node* newNode = new Node(version);
    if (!head) {
        head = tail = newNode;
    }
    else {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }
    size++;
}

VersionNode VersionList::getLatest() {
    if (!tail) return VersionNode();
    return tail->data;
}

VersionNode VersionList::getVersion(int versionNumber) {
    Node* current = head;
    while (current) {
        if (current->data.versionNumber == versionNumber) {
            return current->data;
        }
        current = current->next;
    }
    return VersionNode();
}

bool VersionList::isEmpty() { return size == 0; }
int VersionList::getSize() { return size; }

// UserTable implementation
UserTable::UserEntry::UserEntry(string k, User v) : key(k), value(v), next(nullptr) {}

int UserTable::hashFunction(string key) {
    int hash = 0;
    for (char c : key) {
        hash = (hash * 31 + c) % TABLE_SIZE;
    }
    return hash;
}

UserTable::UserTable() {
    table = new UserEntry * [TABLE_SIZE]();
}

UserTable::~UserTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        UserEntry* entry = table[i];
        while (entry) {
            UserEntry* prev = entry;
            entry = entry->next;
            delete prev;
        }
    }
    delete[] table;
}

void UserTable::insert(string key, User value) {
    int hash = hashFunction(key);
    UserEntry* prev = nullptr;
    UserEntry* entry = table[hash];

    while (entry && entry->key != key) {
        prev = entry;
        entry = entry->next;
    }

    if (!entry) {
        entry = new UserEntry(key, value);
        if (!prev) {
            table[hash] = entry;
        }
        else {
            prev->next = entry;
        }
    }
    else {
        entry->value = value;
    }
}

User* UserTable::get(string key) {
    int hash = hashFunction(key);
    UserEntry* entry = table[hash];

    while (entry) {
        if (entry->key == key) {
            return &entry->value;
        }
        entry = entry->next;
    }
    return nullptr;
}

void UserTable::remove(string key) {
    int hash = hashFunction(key);
    UserEntry* prev = nullptr;
    UserEntry* entry = table[hash];

    while (entry && entry->key != key) {
        prev = entry;
        entry = entry->next;
    }

    if (!entry) return;

    if (!prev) {
        table[hash] = entry->next;
    }
    else {
        prev->next = entry->next;
    }
    delete entry;
}

// FileTable implementation
FileTable::FileEntry::FileEntry(string k) : key(k), value(), next(nullptr) {}

int FileTable::hashFunction(string key) {
    int hash = 0;
    for (char c : key) {
        hash = (hash * 31 + c) % TABLE_SIZE;
    }
    return hash;
}

FileTable::FileTable() {
    table = new FileEntry * [TABLE_SIZE]();
}

FileTable::~FileTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        FileEntry* entry = table[i];
        while (entry) {
            FileEntry* prev = entry;
            entry = entry->next;
            delete prev;
        }
    }
    delete[] table;
}

void FileTable::insert(string key) {
    int hash = hashFunction(key);
    FileEntry* prev = nullptr;
    FileEntry* entry = table[hash];

    while (entry && entry->key != key) {
        prev = entry;
        entry = entry->next;
    }

    if (!entry) {
        entry = new FileEntry(key);
        if (!prev) {
            table[hash] = entry;
        }
        else {
            prev->next = entry;
        }
    }
}

VersionList* FileTable::get(string key) {
    int hash = hashFunction(key);
    FileEntry* entry = table[hash];

    while (entry) {
        if (entry->key == key) {
            return &entry->value;
        }
        entry = entry->next;
    }
    return nullptr;
}

void FileTable::remove(string key) {
    int hash = hashFunction(key);
    FileEntry* prev = nullptr;
    FileEntry* entry = table[hash];

    while (entry && entry->key != key) {
        prev = entry;
        entry = entry->next;
    }

    if (!entry) return;

    if (!prev) {
        table[hash] = entry->next;
    }
    else {
        prev->next = entry->next;
    }
    delete entry;
}

// MetadataTable implementation
MetadataTable::MetadataEntry::MetadataEntry(string k, FileMetadata v) : key(k), value(v), next(nullptr) {}

int MetadataTable::hashFunction(string key) {
    int hash = 0;
    for (char c : key) {
        hash = (hash * 31 + c) % TABLE_SIZE;
    }
    return hash;
}

MetadataTable::MetadataTable() {
    table = new MetadataEntry * [TABLE_SIZE]();
}

MetadataTable::~MetadataTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        MetadataEntry* entry = table[i];
        while (entry) {
            MetadataEntry* prev = entry;
            entry = entry->next;
            delete prev;
        }
    }
    delete[] table;
}

void MetadataTable::insert(string key, FileMetadata value) {
    int hash = hashFunction(key);
    MetadataEntry* prev = nullptr;
    MetadataEntry* entry = table[hash];

    while (entry && entry->key != key) {
        prev = entry;
        entry = entry->next;
    }

    if (!entry) {
        entry = new MetadataEntry(key, value);
        if (!prev) {
            table[hash] = entry;
        }
        else {
            prev->next = entry;
        }
    }
    else {
        entry->value = value;
    }
}

FileMetadata* MetadataTable::get(string key) {
    int hash = hashFunction(key);
    MetadataEntry* entry = table[hash];

    while (entry) {
        if (entry->key == key) {
            return &entry->value;
        }
        entry = entry->next;
    }
    return nullptr;
}

void MetadataTable::remove(string key) {
    int hash = hashFunction(key);
    MetadataEntry* prev = nullptr;
    MetadataEntry* entry = table[hash];

    while (entry && entry->key != key) {
        prev = entry;
        entry = entry->next;
    }

    if (!entry) return;

    if (!prev) {
        table[hash] = entry->next;
    }
    else {
        prev->next = entry->next;
    }
    delete entry;
}

// NotificationList implementation
NotificationList::Node::Node(Notification d) : data(d), next(nullptr) {}

NotificationList::NotificationList() : head(nullptr), tail(nullptr), size(0) {}

NotificationList::~NotificationList() {
    Node* current = head;
    while (current) {
        Node* next = current->next;
        delete current;
        current = next;
    }
}

void NotificationList::add(Notification notification) {
    Node* newNode = new Node(notification);
    if (!head) {
        head = tail = newNode;
    }
    else {
        tail->next = newNode;
        tail = newNode;
    }
    size++;
}

void NotificationList::displayAll() {
    Node* current = head;
    int count = 1;
    while (current) {
        setColor(8); cout << count++ << ". " << current->data.getMessage() << endl;
        current = current->next;
    }
    setColor(7);
}

bool NotificationList::isEmpty() { return size == 0; }
int NotificationList::getSize() { return size; }

void NotificationList::clear() {
    Node* current = head;
    while (current) {
        Node* next = current->next;
        delete current;
        current = next;
    }
    head = tail = nullptr;
    size = 0;
}

// UserNotificationTable implementation
UserNotificationTable::UserNotificationEntry::UserNotificationEntry(string u) : username(u), next(nullptr) {}

int UserNotificationTable::hashFunction(string key) {
    int hash = 0;
    for (char c : key) {
        hash = (hash * 31 + c) % TABLE_SIZE;
    }
    return hash;
}

UserNotificationTable::UserNotificationTable() {
    table = new UserNotificationEntry * [TABLE_SIZE]();
}

UserNotificationTable::~UserNotificationTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        UserNotificationEntry* entry = table[i];
        while (entry) {
            UserNotificationEntry* prev = entry;
            entry = entry->next;
            delete prev;
        }
    }
    delete[] table;
}

NotificationList* UserNotificationTable::getNotifications(string username) {
    int hash = hashFunction(username);
    UserNotificationEntry* entry = table[hash];

    while (entry) {
        if (entry->username == username) {
            return &entry->notifications;
        }
        entry = entry->next;
    }

    // Create new entry if not found
    UserNotificationEntry* newEntry = new UserNotificationEntry(username);
    newEntry->next = table[hash];
    table[hash] = newEntry;
    return &newEntry->notifications;
}

// NotificationManager implementation
void NotificationManager::addNotification(string username, string message) {
    NotificationList* notifications = userNotifications.getNotifications(username);
    notifications->add(Notification(message));
}

void NotificationManager::displayNotifications(string username) {
    setColor(9); cout << "\n*--------Notifications*----------\n"; setColor(7);
    NotificationList* notifications = userNotifications.getNotifications(username);

    if (notifications->isEmpty()) {
        setColor(12); cout << "No notifications.\n"; setColor(7);
        return;
    }

    setColor(10); cout << "You have " << notifications->getSize() << " notifications\n"; setColor(7);
    notifications->displayAll();
}

void NotificationManager::clearNotifications(string username) {
    NotificationList* notifications = userNotifications.getNotifications(username);
    notifications->clear();
}

// UserManager implementation
UserManager::UserManager() : currentUser(nullptr) {}

bool UserManager::isLoggedIn() { return currentUser != nullptr; }
User* UserManager::getCurrentUser() { return currentUser; }

bool UserManager::login(string username, string password) {
    User* user = users.get(username);
    if (user && user->verifyPassword(password)) {
        currentUser = user;
        return true;
    }
    return false;
}

void UserManager::logout() {
    currentUser = nullptr;
}

bool UserManager::signUp(string username, string password, string question, string answer) {
    if (users.get(username)) return false;

    User newUser(username, password, question, answer);
    users.insert(username, newUser);
    return true;
}

string UserManager::getSecurityQuestion(string username) {
    User* user = users.get(username);
    return user ? user->getSecurityQuestion() : "";
}

bool UserManager::resetPassword(string username, string answer, string newPassword) {
    User* user = users.get(username);
    if (user && user->checkSecurityAnswer(answer)) {
        User updatedUser = *user;
        updatedUser = User(username, newPassword, user->getSecurityQuestion(), answer);
        users.insert(username, updatedUser);
        return true;
    }
    return false;
}

// FileManager implementation
FileManager::FileManager(DeletedFileStack* bin) : notificationManager(nullptr), recycleBin(bin) {}

void FileManager::setNotificationManager(NotificationManager* nm) {
    notificationManager = nm;
}

string FileManager::readFile(string path, string username) {
    FileMetadata* meta = fileMetadata.get(path);
    if (!meta || !meta->hasPermission(username, "r")) {
        cout << "Access denied or file not found: " << path << endl;
        return "";
    }

    meta->setLastReadTime();
    VersionList* versions = fileVersions.get(path);
    if (!versions || versions->isEmpty()) {
        cout << "No versions available for: " << path << endl;
        return "";
    }

    return versions->getLatest().content;
}

bool FileManager::shareFile(string path, string owner, string recipient, string permissions) {
    FileMetadata* meta = fileMetadata.get(path);
    if (!meta || meta->getOwner() != owner) return false;

    meta->setPermissions(permissions);
    meta->setLastSharedTime();

    if (notificationManager) {
        string msg = owner + " shared " + meta->getName() + " with you (" + permissions +
            " permissions) at " + meta->getLastSharedTime();
        notificationManager->addNotification(recipient, msg);
    }

    cout << "File shared at: " << meta->getLastSharedTime() << endl;
    return true;
}

bool FileManager::createFile(string path, string name, string type, string owner, string content) {
    if (fileVersions.get(path)) {
        cout << "File already exists at path: " << path << endl;
        return false;
    }

    FileMetadata meta(name, path, type, owner);
    fileMetadata.insert(path, meta);

    VersionNode initialVersion(1, content);
    fileVersions.insert(path);
    VersionList* versions = fileVersions.get(path);
    versions->append(initialVersion);

    cout << "File created with path: " << path << endl;
    return true;
}

bool FileManager::updateFile(string path, string newContent, string username) {
    FileMetadata* meta = fileMetadata.get(path);
    if (!meta || meta->getOwner() != username) {
        cout << "Permission denied: " << path << endl;
        return false;
    }

    VersionList* versions = fileVersions.get(path);
    if (!versions) return false;

    VersionNode newVersion(versions->getSize() + 1, newContent);
    versions->append(newVersion);
    return true;
}

bool FileManager::deleteFile(string path, string username) {
    FileMetadata* meta = fileMetadata.get(path);
    if (!meta || meta->getOwner() != username) return false;

    string content = readFile(path, username);
    if (content.empty()) return false;

    fileVersions.remove(path);
    fileMetadata.remove(path);

    recycleBin->push(DeletedFile(path, content, username));
    return true;
}

bool FileManager::rollbackFile(string path, int versionNumber, string username) {
    FileMetadata* meta = fileMetadata.get(path);
    if (!meta || meta->getOwner() != username) return false;

    VersionList* versions = fileVersions.get(path);
    if (!versions) return false;

    VersionNode version = versions->getVersion(versionNumber);
    if (version.versionNumber == 0) return false;

    VersionNode rollbackVersion(versions->getSize() + 1, version.content);
    versions->append(rollbackVersion);
    return true;
}

FileMetadata* FileManager::getFileMetadata(string path) {
    return fileMetadata.get(path);
}

void FileManager::notifyOwnerOnAccess(const string& path, const string& accessor) {
    if (!notificationManager) return;
    FileMetadata* meta = getFileMetadata(path);
    if (meta && accessor != meta->getOwner()) {
        string msg = "Your file '" + path + "' was accessed by " + accessor + ".";
        notificationManager->addNotification(accessor, msg);
    }
}

// DirectoryManager implementation
string* DirectoryManager::splitPath(string path, int& count) {
    count = 0;
    bool inPart = false;
    for (char c : path) {
        if (c == '/') {
            inPart = false;
        }
        else if (!inPart) {
            count++;
            inPart = true;
        }
    }

    string* parts = new string[count];
    int index = 0;
    int start = 0;
    int end = path.find('/');
    while (end != string::npos) {
        if (end > start) {
            parts[index++] = path.substr(start, end - start);
        }
        start = end + 1;
        end = path.find('/', start);
    }
    if (start < path.length()) {
        parts[index] = path.substr(start);
    }

    return parts;
}

TreeNode* DirectoryManager::resolvePath(string* parts, int count) {
    TreeNode* node = (parts[0] == "root") ? root : current;
    int startIdx = (parts[0] == "root") ? 1 : 0;

    for (int i = startIdx; i < count; i++) {
        node = node->findChild(parts[i]);
        if (!node) {
            return nullptr;
        }
    }
    return node;
}

DirectoryManager::DirectoryManager(FileManager* fm) : fileManager(fm), root(new TreeNode()), current(root) {
    root->setName("root");
}

DirectoryManager::~DirectoryManager() {
    delete root;
}

bool DirectoryManager::createFolder(string name) {
    if (current->findChild(name)) {
        return false;
    }
    TreeNode* newFolder = new TreeNode(name, "folder", current);
    current->addChild(newFolder);
    return true;
}

bool DirectoryManager::createFile(string name, string type) {
    if (current->findChild(name)) {
        return false;
    }
    TreeNode* newFile = new TreeNode(name, type, current);
    current->addChild(newFile);
    return true;
}

bool DirectoryManager::navigateTo(string path) {
    int count = 0;
    string* parts = splitPath(path, count);
    TreeNode* target = resolvePath(parts, count);
    delete[] parts;

    if (target && target->getType() == "folder") {
        current = target;
        return true;
    }
    return false;
}

string DirectoryManager::getCurrentPath() {
    const int MAX_DEPTH = 100;
    TreeNode* pathNodes[MAX_DEPTH];
    int depth = 0;
    TreeNode* node = current;

    // Collect path nodes
    while (node && node != root && depth < MAX_DEPTH) {
        pathNodes[depth++] = node;
        node = node->getParent();
    }

    // Build path string
    string path = "Drive";
    for (int i = depth - 1; i >= 0; i--) {
        path += "/" + pathNodes[i]->getName();
    }
    return path;
}

void DirectoryManager::cleanupDeletedFiles(const string& currentUsername) {
    int childCount = current->getChildCount();
    for (int i = 0; i < childCount; ) {
        TreeNode* child = current->getChild(i);
        if (child->getType() != "folder") {
            string path = getCurrentPath() + "/" + child->getName();
            FileMetadata* meta = fileManager->getFileMetadata(path);
            if (!meta || meta->getOwner() != currentUsername) {
                // Remove the file node
                delete child;
                // Shift remaining children
                for (int j = i; j < childCount - 1; j++) {
                    current->getChildren()[j] = current->getChildren()[j + 1];
                }
                current->setChildCount(--childCount);
                continue;
            }
        }
        i++;
    }
}

void DirectoryManager::listContents(const string& currentUsername) {
    cleanupDeletedFiles(currentUsername);
    cout << "Contents of " << getCurrentPath() << ":\n";

    int childCount = current->getChildCount();
    if (childCount == 0) {
        cout << " No File.\n";
        return;
    }

    for (int i = 0; i < childCount; i++) {
        TreeNode* child = current->getChild(i);
        if (child->getType() == "folder") {
            cout << "  " << child->getName() << "/\n";
        }
        else {
            string path = getCurrentPath() + "/" + child->getName();
            FileMetadata* meta = fileManager->getFileMetadata(path);
            if (meta && meta->getOwner() == currentUsername) {
                cout << "  " << child->getName() << "\n";
            }
        }
    }
}

TreeNode* DirectoryManager::getCurrent() { return current; }

    void FileSystem::tital() {
        setColor(14);
        cout << R"(
   ____                   _        _____  _____   _____ __      __ ______  ______ _____ _      ______     _____ __     __ _____ _______ ______ __  __ 
  / ___| ___   ___   __ _| | ___  |  __ \|  __ \ |_   _|\ \    / /|  ____||  ____|_   _| |    |  ____    / ____|\ \   / // ____|__   __|  ____|  \/  |
 | |  _ / _ \ / _ \ / _` | |/ _ \ | |  | | |__) |  | |   \ \  / / | |__   | |__    | | | |    | |__     | (___   \ \_/ /| (___    | |  | |__  | \  / |
 | |_| | (_) | (_) | (_| | |  __/ | |  | |  _  /   | |    \ \/ /  |  __|  |  __|   | | | |    |  __|     \___ \   \   /  \___ \   | |  |  __| | |\/| |
  \____|\___/ \___/ \__, |_|\___| | |__| | | \ \  _| |_    \  /   | |____ | |     _| |_| |____| |____    ____) |   | |   ____) |  | |  | |____| |  | |
                   |___/          |_____/|_|  \_\|_____|    \/    |______||_|    |_____|______|______|  |_____/    |_|  |_____/   |_|  |______|_|  |_|
)" << "\n\n";
        setColor(7);
    }

  

    string FileSystem::getHiddenInput(const string& prompt) {
        setColor(11); cout << prompt;
        string password; setColor(2);
        char ch;
        while ((ch = _getch()) != '\r') {  // '\r' is Enter key
            if (ch == '\b') {  // Handle backspace
                if (!password.empty()) {
                    cout << "\b \b";
                    password.pop_back();
                }
            }
            else if (isprint(ch)) {
                password += ch;
                cout << '*';
            }
        }
        cout << '\n';
        return password;
    }
    void FileSystem::showAuthMenu() {

        while (!userManager.isLoggedIn()) {
            system("cls");
            tital();
            setColor(9); cout << "\n*------Google Drive File System--------*\n"; setColor(11);
            cout << "1. Login\n";
            cout << "2. Sign Up\n";
            cout << "3. Forgot Password\n";
            cout << "4. Exit\n";
            cout << "\n*-------------------------------------*\n";
            cout << "Choose an option: "; setColor(7);

            char choice;
            setColor(2); cin >> choice; setColor(7);
            cin.ignore();

            system("cls"); tital();
            switch (choice) {
            case '1': {
                setColor(11); cout << "Username: ";
                string username;
                setColor(2); getline(cin, username);
                string password = getHiddenInput("Password: "); setColor(7);
                system("cls"); tital();
                if (userManager.login(username, password)) {
                    setColor(10); cout << "Login successful!\n"; setColor(7);
                    setColor(8);  notificationManager.addNotification(username, "Welcome back, " + username + "!"); setColor(7);
                }
                else {
                    setColor(12); cout << "Invalid username or password.\n"; setColor(7);
                }
                break;
            }
            case '2': {
                setColor(11); cout << "Username: ";
                string username;
                setColor(2); getline(cin, username);
                setColor(11); cout << "Password: ";
                string password;
                setColor(2); getline(cin, password);
                setColor(14); cout << "Security Question: ";
                string question;
                setColor(2); getline(cin, question);
                setColor(14);  cout << "Security Answer: "; setColor(7);
                string answer;
                setColor(2);  getline(cin, answer); setColor(7);

                if (userManager.signUp(username, password, question, answer)) {
                    setColor(10); cout << "Registration successful!\n"; setColor(7);
                }
                else {
                    setColor(12); cout << "Username already exists.\n"; setColor(7);
                }
                break;
            }
            case '3': {
                setColor(11); cout << "Username: ";
                string username;
                setColor(2); getline(cin, username);

                string question = userManager.getSecurityQuestion(username);
                if (question.empty()) {
                    setColor(12); cout << "User not found.\n"; setColor(10);
                    break;
                }

                setColor(14); cout << "Security Question: " << question << "\n"; setColor(7);
                cout << "Answer: ";
                string answer;
                setColor(2); getline(cin, answer);
                setColor(11); cout << "New Password: ";
                string newPassword;
                setColor(2); getline(cin, newPassword);

                if (userManager.resetPassword(username, answer, newPassword)) {
                    setColor(10); cout << "Password reset successful!\n"; setColor(7);
                }
                else {
                    setColor(12); cout << "Incorrect answer.\n"; setColor(7);
                }
                break;
            }
            case '4':
                
                exit(0);
            default:
                setColor(12); cout << "Invalid choice. Try again.\n"; setColor(7);
            }
            Sleep(1200);
        }
    }

    void FileSystem::fileOperationsMenu() {

        string username = userManager.getCurrentUser()->getUsername();

        while (true) {
            system("cls"); tital();
            setColor(9); cout << "\n*--------File Operations--------*\n"; setColor(11);
            cout << "1. Create File\n";
            cout << "2. Read File\n";
            cout << "3. Update File\n";
            cout << "4. Delete File\n";
            cout << "5. Rollback File Version\n";
            cout << "6. Back to Main Menu\n";
            cout << "\n*-----------------------------*\n";

            cout << "Choose an option: ";

            char choice;
            setColor(2); cin >> choice; setColor(7);
            cin.ignore();
            switch (choice) {
                system("cls"); tital();
            case '1': {
                setColor(11); cout << "File name: ";
                string name;
                setColor(2);  getline(cin, name);
                setColor(11); cout << "File type: ";
                string type;
                setColor(2);  getline(cin, type);
                setColor(11); cout << "Initial content: ";
                string content;
                setColor(2); getline(cin, content); setColor(7);

                string path = dirManager.getCurrentPath() + "/" + name;
                if (fileManager.createFile(path, name, type, username, content)) {
                    dirManager.createFile(name, type);
                    FileMetadata* meta = fileManager.getFileMetadata(path);
                    if (meta) {
                        setColor(10); cout << "File created successfully at: " << meta->getCreationTime() << "\n"; setColor(7);
                    }
                    else {
                        setColor(10); cout << "File created successfully.\n"; setColor(7);
                    }
                }
                else {
                    setColor(12); cout << "File creation failed.\n"; setColor(7);
                }

                break;
            }
            case '2': {
                dirManager.listContents(username);
                setColor(11); cout << "File name to read: ";
                string name;
                setColor(2); getline(cin, name); setColor(7);
                string path = dirManager.getCurrentPath() + "/" + name;
                string content = fileManager.readFile(path, username);
                if (!content.empty()) {
                    setColor(10); FileMetadata* meta = fileManager.getFileMetadata(path);
                    if (meta) {
                        setColor(11); cout << "File last read at: " << meta->getLastReadTime() << "\n";
                    }
                    setColor(8);  cout << "File content:\n" << content << "\n";
                    recentFiles.enqueue(path); setColor(7);
                    fileManager.notifyOwnerOnAccess(path, username); // <-- Add here
                }
                else {
                    setColor(12);  cout << "Failed to read file.\n"; setColor(7);
                }
                break;
            }
            case '3': {
                dirManager.listContents(username);
                setColor(11); cout << "File name to update: ";
                string name;
                setColor(2);  getline(cin, name);
                setColor(11); cout << "New content: ";
                string content;
                setColor(2); getline(cin, content);
                string path = dirManager.getCurrentPath() + "/" + name;
                if (fileManager.updateFile(path, content, username)) {
                    setColor(10); cout << "File updated successfully.\n"; setColor(7);
                    recentFiles.enqueue(path);
                }
                else {
                    setColor(12);  cout << "Update failed.\n"; setColor(7);
                }
                break;
            }
            case '4': {
                dirManager.listContents(username);
                setColor(11); cout << "File name to delete: ";
                string name;
                setColor(2); getline(cin, name);
                string path = dirManager.getCurrentPath() + "/" + name;
                string content = fileManager.readFile(path, username);
                if (fileManager.deleteFile(path, username)) {
                    recycleBin.push(DeletedFile(path, content, username));
                    setColor(12);   cout << "File moved to recycle bin.\n"; setColor(7);
                }
                else {
                    setColor(12); cout << "Deletion failed.\n"; setColor(7);
                }
                break;
            }
            case '5': {
                dirManager.listContents(username);
                setColor(11); cout << "File name to rollback: ";
                string name;
                setColor(2); getline(cin, name);
                setColor(11); cout << "Version number: ";
                int version;
                setColor(2); cin >> version;
                cin.ignore();
                string path = dirManager.getCurrentPath() + "/" + name;
                if (fileManager.rollbackFile(path, version, username)) {
                    setColor(10);   cout << "Rollback successful.\n"; setColor(7);
                    recentFiles.enqueue(path);
                }
                else {
                    setColor(12); cout << "Rollback failed.\n"; setColor(7);
                }
                break;
            }
            case '6':
                return;
            default:
                setColor(12); cout << "Invalid choice. Try again.\n"; setColor(7);
            }
            system("pause");
        }
    }


    void FileSystem::directoryOperationsMenu() {

        while (true) {
            system("cls"); tital();
            setColor(9); cout << "\n*----------Directory Operations----------*\n"; setColor(11);
            cout << "1. Create Folder\n";
            cout << "2. Navigate To\n";
            cout << "3. List Contents\n";
            cout << "4. Back to Main Menu\n";
            cout << "\n*---------------------------------------*\n";
            cout << "Choose an option: "; setColor(7);

            char choice;
            setColor(2); cin >> choice;
            cin.ignore();

            system("cls"); tital();
            switch (choice) {
            case '1': {
                setColor(11); cout << "Folder name: ";
                string name;
                setColor(2); getline(cin, name);
                if (dirManager.createFolder(name)) {
                    setColor(10); cout << "Folder created successfully.\n"; setColor(7);
                }
                else {
                    setColor(12); cout << "Folder already exists.\n"; setColor(7);
                }
                break;
            }
            case '2': {
                setColor(11); cout << "Path to navigate (e.g., root/folder1): ";
                string path;
                setColor(2); getline(cin, path);
                if (dirManager.navigateTo(path)) {
                    setColor(11); cout << "Now in: " << dirManager.getCurrentPath() << "\n"; setColor(7);
                }
                else {
                    setColor(12); cout << "Invalid path.\n"; setColor(7);
                }
                break;
            }
            case '3':
                dirManager.listContents(userManager.getCurrentUser()->getUsername());
                break;
            case '4':
                return;
            default:
                setColor(12); cout << "Invalid choice. Try again.\n"; setColor(7);
            }
            system("pause");
        }
    }

    void FileSystem::recycleBinMenu() {

        string username = userManager.getCurrentUser()->getUsername();

        while (true) {
            system("cls"); tital();

            setColor(9);  cout << "\n*---------Recycle Bin (" << username << ")---------*\n"; setColor(11);
            cout << "1. Restore Latest File\n";
            cout << "2. Auto Clean Old Files\n";
            cout << "3. Back to Main Menu\n";
            cout << "\n*--------------------------------------------*\n";

            cout << "Choose an option: "; setColor(7);

            int choice;
            setColor(2); cin >> choice; setColor(7);
            cin.ignore();

            switch (choice) {
            case 1: {
                setColor(14); DeletedFile file = recycleBin.pop(username);
                if (!file.getPath().empty()) {
                    size_t lastSlash = file.getPath().rfind('/');
                    string name = (lastSlash == string::npos) ? file.getPath()
                        : file.getPath().substr(lastSlash + 1);

                    fileManager.createFile(file.getPath(), name, "file",
                        username, file.getContent());
                    setColor(10);  cout << "File restored: " << name << "\n"; setColor(7);
                }
                else {
                    cout << "Your recycle bin is empty.\n";
                }
                setColor(7); break;
            }
            case 2: {
                while (!recycleBin.isEmpty(username)) {
                    recycleBin.pop(username);
                }
                setColor(14); cout << "Your old files cleaned from recycle bin.\n"; setColor(7);
                break;
            }
            case 3:
                return;
            default:
                setColor(12); cout << "Invalid choice. Try again.\n"; setColor(7);
            }
            Sleep(1200);
        }
    }

    void FileSystem::shareFileMenu() {

        system("cls"); tital();
        setColor(9); cout << "\n*------------Share File-------------*\n";
        dirManager.listContents(userManager.getCurrentUser()->getUsername());
        setColor(11); cout << "File name to share: ";
        string name;
        setColor(2); getline(cin, name);
        setColor(11); cout << "Recipient username: ";
        string recipient;
        setColor(2); getline(cin, recipient);
        setColor(11); cout << "Permissions (r/w/rw): ";
        string permissions;
        setColor(2); getline(cin, permissions); setColor(7);

        string path = dirManager.getCurrentPath() + "/" + name;
        string owner = userManager.getCurrentUser()->getUsername();

        if (fileManager.shareFile(path, owner, recipient, permissions)) {
            setColor(10); cout << "File shared successfully. Notification sent to " << recipient << ".\n"; setColor(7);
        }
        else {
            setColor(12); cout << "Failed to share file. You must be the owner.\n"; setColor(7);
        }
        setColor(7);
        Sleep(1200);
    }

    void FileSystem::notificationsMenu() {
        system("cls");
        tital();
        notificationManager.displayNotifications(userManager.getCurrentUser()->getUsername());

        system("pause");
    }

    void FileSystem::showMainMenu() {

        string username = userManager.getCurrentUser()->getUsername();

        while (userManager.isLoggedIn()) {
            system("cls"); tital();
            setColor(9); cout << "\n*-----------Google Drive File System-----------*\n"; setColor(7);
            setColor(11); cout << "Logged in as: " << username << "\n";
            cout << "Current directory: " << dirManager.getCurrentPath() << "\n\n";

            cout << "1. File Operations\n";
            cout << "2. Directory Operations\n";
            cout << "3. Recycle Bin\n";
            cout << "4. Recent Files\n";
            cout << "5. Share File\n";
            cout << "6. Notifications\n";
            cout << "7. Logout\n";
            cout << "\n*------------------------------------------*\n";

            cout << "Choose an option: ";

            char choice;
            setColor(2); cin >> choice; setColor(7);
            cin.ignore();

            switch (choice) {
            case '1': fileOperationsMenu(); break;
            case '2': directoryOperationsMenu(); break;
            case '3': recycleBinMenu(); break;
            case '4': setColor(11); cout << "Recent file: " << recentFiles.displayRecent() << "\n"; setColor(7); system("pause"); break;
            case '5': shareFileMenu(); break;
            case '6': notificationsMenu(); break;
            case '7': userManager.logout(); system("cls"); break;
            default:setColor(12); cout << "Invalid choice. Try again.\n"; setColor(7); Sleep(1200);
            }

        }
    }


    FileSystem::FileSystem() : fileManager(&recycleBin), dirManager(&fileManager) {
        fileManager.setNotificationManager(&notificationManager);
    }

    void FileSystem::run() {
        while (true) {
            if (!userManager.isLoggedIn()) {
                showAuthMenu();
            }
            else {
                showMainMenu();
            }
        }
    }
