#pragma once
#include <iostream>
#include <string>
#include <ctime>
#include <cstring>
#include <conio.h>
#include <cstdlib>
#include <Windows.h>

using namespace std;

// Utility Functions
void setColor(int color);
string getCurrentDateTime();

// Data Classes
class User {
private:
    string username;
    string password;
    string securityQuestion;
    string securityAnswer;
    string lastLogin;
    string lastLogout;

public:
    User();
    User(string u, string p, string q, string a);
    string getUsername();
    string getSecurityQuestion();
    bool checkSecurityAnswer(string answer);
    bool verifyPassword(string p);
    void setLastLogin();
    void setLastLogout();
};

class FileMetadata {
private:
    string name;
    string path;
    int size;
    string type;
    string dateCreated;
    string owner;
    string permissions;
    string creationTime;
    string lastReadTime;
    string lastSharedTime;

public:
    FileMetadata();
    FileMetadata(string n, string p, string t, string o);
    void setLastReadTime();
    void setLastSharedTime();
    string getLastReadTime();
    string getLastSharedTime();
    string getOwner();
    string getName();
    string getPath();
    string getType();
    string getPermissions();
    string getCreationTime();
    void setPermissions(string p);
    bool hasPermission(string username, string requiredPermission);
};

class VersionNode {
public:
    int versionNumber;
    string timestamp;
    string content;
    VersionNode();
    VersionNode(int v, string c);
};

class TreeNode {
private:
    string name;
    string type;
    TreeNode* parent;
    TreeNode** children;
    int childCount;
    int capacity;
    void resizeChildren();

public:
    TreeNode();
    TreeNode(string n, string t, TreeNode* p);
    ~TreeNode();
    void setName(string n);
    string getName();
    string getType();
    TreeNode* getParent();
    int getChildCount();
    TreeNode* getChild(int index);
    void addChild(TreeNode* child);
    TreeNode* findChild(string name);
    void setChildCount(int count);
    TreeNode** getChildren();
};

class DeletedFile {
private:
    string path;
    string content;
    string deletionTime;
    string owner;

public:
    DeletedFile();
    DeletedFile(string p, string c, string o);
    string getPath() const;
    string getContent() const;
    string getDeletionTime() const;
    string getOwner() const;
};

class Notification {
private:
    string message;
    string timestamp;
    bool read;

public:
    Notification(string msg);
    string getMessage() const;
    string getTimestamp();
    bool isRead();
    void markAsRead();
};

// Data Structures
class DeletedFileStack {
private:
    struct Node {
        DeletedFile data;
        Node* next;
        Node(DeletedFile d);
    };

    Node* top;
    int size;
    void removeNode(Node* node, Node* prev);

public:
    DeletedFileStack();
    ~DeletedFileStack();
    void push(DeletedFile file);
    DeletedFile pop(string currentUser);
    bool isEmpty(string currentUser = "") const;
    int getCount(string currentUser) const;
    void clear(string currentUser = "");
    void displayAll(string currentUser) const;
};

class RecentFilesQueue {
private:
    struct Node {
        string data;
        Node* next;
        Node(string d);
    };

    Node* front;
    Node* rear;
    int size;
    const int capacity = 10;

public:
    RecentFilesQueue();
    ~RecentFilesQueue();
    void enqueue(string path);
    string dequeue();
    string displayRecent();
    bool isEmpty();
};

class VersionList {
private:
    struct Node {
        VersionNode data;
        Node* prev;
        Node* next;
        Node(VersionNode d);
    };

    Node* head;
    Node* tail;
    int size;

public:
    VersionList();
    ~VersionList();
    void append(VersionNode version);
    VersionNode getLatest();
    VersionNode getVersion(int versionNumber);
    bool isEmpty();
    int getSize();
};

class UserTable {
private:
    struct UserEntry {
        string key;
        User value;
        UserEntry* next;
        UserEntry(string k, User v);
    };

    static const int TABLE_SIZE = 100;
    UserEntry** table;
    int hashFunction(string key);

public:
    UserTable();
    ~UserTable();
    void insert(string key, User value);
    User* get(string key);
    void remove(string key);
};

class FileTable {
private:
    struct FileEntry {
        string key;
        VersionList value;
        FileEntry* next;
        FileEntry(string k);
    };

    static const int TABLE_SIZE = 100;
    FileEntry** table;
    int hashFunction(string key);

public:
    FileTable();
    ~FileTable();
    void insert(string key);
    VersionList* get(string key);
    void remove(string key);
};

class MetadataTable {
private:
    struct MetadataEntry {
        string key;
        FileMetadata value;
        MetadataEntry* next;
        MetadataEntry(string k, FileMetadata v);
    };

    static const int TABLE_SIZE = 100;
    MetadataEntry** table;
    int hashFunction(string key);

public:
    MetadataTable();
    ~MetadataTable();
    void insert(string key, FileMetadata value);
    FileMetadata* get(string key);
    void remove(string key);
};

class NotificationList {
private:
    struct Node {
        Notification data;
        Node* next;
        Node(Notification d);
    };

    Node* head;
    Node* tail;
    int size;

public:
    NotificationList();
    ~NotificationList();
    void add(Notification notification);
    void displayAll();
    bool isEmpty();
    int getSize();
    void clear();
};

class UserNotificationTable {
private:
    struct UserNotificationEntry {
        string username;
        NotificationList notifications;
        UserNotificationEntry* next;
        UserNotificationEntry(string u);
    };

    static const int TABLE_SIZE = 100;
    UserNotificationEntry** table;
    int hashFunction(string key);

public:
    UserNotificationTable();
    ~UserNotificationTable();
    NotificationList* getNotifications(string username);
};

class NotificationManager {
private:
    UserNotificationTable userNotifications;

public:
    void addNotification(string username, string message);
    void displayNotifications(string username);
    void clearNotifications(string username);
};

class UserManager {
private:
    UserTable users;
    User* currentUser;

public:
    UserManager();
    bool isLoggedIn();
    User* getCurrentUser();
    bool login(string username, string password);
    void logout();
    bool signUp(string username, string password, string question, string answer);
    string getSecurityQuestion(string username);
    bool resetPassword(string username, string answer, string newPassword);
};

class FileManager {
private:
    FileTable fileVersions;
    MetadataTable fileMetadata;
    NotificationManager* notificationManager;
    DeletedFileStack* recycleBin;

public:
    FileManager(DeletedFileStack* bin);
    void setNotificationManager(NotificationManager* nm);
    string readFile(string path, string username);
    bool shareFile(string path, string owner, string recipient, string permissions);
    bool createFile(string path, string name, string type, string owner, string content);
    bool updateFile(string path, string newContent, string username);
    bool deleteFile(string path, string username);
    bool rollbackFile(string path, int versionNumber, string username);
    FileMetadata* getFileMetadata(string path);
    void notifyOwnerOnAccess(const string& path, const string& accessor);
};

class DirectoryManager {
private:
    TreeNode* root;
    TreeNode* current;
    FileManager* fileManager;
    string* splitPath(string path, int& count);
    TreeNode* resolvePath(string* parts, int count);

public:
    DirectoryManager(FileManager* fm);
    ~DirectoryManager();
    bool createFolder(string name);
    bool createFile(string name, string type);
    bool navigateTo(string path);
    string getCurrentPath();
    void cleanupDeletedFiles(const string& currentUsername);
    void listContents(const string& currentUsername);
    TreeNode* getCurrent();
};

class FileSystem {
private:
    UserManager userManager;
    FileManager fileManager;
    DirectoryManager dirManager;
    DeletedFileStack recycleBin;
    RecentFilesQueue recentFiles;
    NotificationManager notificationManager;

    void tital();
  
    string getHiddenInput(const string& prompt);
    void showAuthMenu();
    void fileOperationsMenu();
    void directoryOperationsMenu();
    void recycleBinMenu();
    void shareFileMenu();
    void notificationsMenu();
    void showMainMenu();

public:
    FileSystem();
    void run();
};

