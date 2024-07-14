#pragma once

#include <string>
#include <vector>

class FS_Node{
  public:
    std::string name;
    int type;
    FS_Node *left = nullptr, *right = nullptr;

    FS_Node(std::string name, int type) : name(name), type(type) {}
};

class File : public FS_Node{
  public: 
    std::string buffer; 

    File(std::string filename) : FS_Node(filename, 0) {}
    File(std::string filename, std::string buffer) : FS_Node(filename, 0), buffer(buffer) {}
};

class Folder : public FS_Node{
  public:  
    FS_Node *subFoldersPtr = nullptr;
    
    Folder(std::string foldername) : FS_Node(foldername, 1), subFoldersPtr(nullptr) {}; 
};

class SL_Node{
  public: 
    Folder *folderPtr = nullptr;
    SL_Node *next = nullptr; 

    SL_Node(Folder *ptr) : folderPtr(ptr) {}; 
};

class FileSystem {
  private: 
    Folder *root = nullptr;
    SL_Node *currFolder = nullptr;  

  public: 

    FileSystem() {
      root = new Folder("/");
      currFolder = new SL_Node(root);
    }
    
    std::string pf() {
      std::string folderName = currFolder->folderPtr->name;
      return folderName;
    }

    bool createFile(std::string filename) {

      Folder *folderPtr = currFolder->folderPtr;
      FS_Node *subFoldersPtr = folderPtr->subFoldersPtr; 

      File *file = new File(filename);
      
      if(subFoldersPtr == nullptr) {
        folderPtr->subFoldersPtr = file;
        return true;
      } 

      FS_Node *previousNode = nullptr;  

      while(subFoldersPtr != nullptr) {
        if(subFoldersPtr->name == file->name) return false;

        previousNode = subFoldersPtr;
        if(subFoldersPtr->name < file->name) {
          subFoldersPtr = subFoldersPtr->right;
        }
        else {
          subFoldersPtr = subFoldersPtr->left;
        }
      }
      
      if(previousNode->name < file->name) {
        previousNode->right = file;
      }
      else {
        previousNode->left = file;
      }

      return true;
    }

    bool createFolder(std::string foldername) {
      Folder *folderPtr = currFolder->folderPtr;
      FS_Node *subFoldersPtr = folderPtr->subFoldersPtr;
      
      Folder *folder = new Folder(foldername);

      if(subFoldersPtr == nullptr) {
        folderPtr->subFoldersPtr = folder;
        return true;
      }

      FS_Node *previousNode = nullptr; 

      while(subFoldersPtr != nullptr) {
        if(subFoldersPtr->name == folder->name) return false;

        previousNode = subFoldersPtr;
        if(folder->name > subFoldersPtr->name) {
          subFoldersPtr = subFoldersPtr->right;
        }
        else {
          subFoldersPtr = subFoldersPtr->left;
        }
      }

      if(folder->name > previousNode->name) {
        previousNode->right = folder;
      }
      else {
        previousNode->left = folder;
      }

      return true;
    }
  
    std::vector<std::string> ls() {
      std::vector<std::string> arr;

      // Inorder Tree Traversal by Morris Threading
      FS_Node *head = currFolder->folderPtr->subFoldersPtr;

      while(head != nullptr) {
        if(head->left != nullptr) {
          FS_Node *predecessor = head->left; 

          while(predecessor->right != nullptr && predecessor->right != head) {
            predecessor = predecessor->right;
          }

          if(predecessor->right == nullptr) {
            predecessor->right = head; 
            head = head->left;
          }
          else{
            arr.push_back(head->name);
            predecessor->right = nullptr; 
            head = head->right;
          }
        }
        else {
          arr.push_back(head->name);
          head = head->right;
        }
      }
    
      return arr;
    }
  
    SL_Node* SL_Node_copy() {
      SL_Node *copy = new SL_Node(currFolder->folderPtr);
      SL_Node *originalPtr = currFolder->next;
      SL_Node *iterator = copy;

      while(originalPtr != nullptr) {
        iterator->next = new SL_Node(originalPtr->folderPtr);
        originalPtr = originalPtr->next; 
        iterator = iterator->next;
      }

      return copy;
    }

    void changeDirectory(std::vector<std::string> path) {
      SL_Node* currPath = SL_Node_copy();

      for(int i=0;i<path.size();i++) {
        if(path[i] == "..") {
          if(currPath->folderPtr->name == "/") {
            continue;
          }
          else {
            SL_Node *prev = currPath; 
            currPath = currPath->next;
            delete prev;
            prev = nullptr; 
          }
        }
        else {
          FS_Node *folderPtr = search(currPath, path[i]);
          if(folderPtr == nullptr || (folderPtr != nullptr && folderPtr->type == 0)) {
            // Deleting the newly created SL_Node list
            while(currPath != nullptr) {
              SL_Node *prev = currPath; 
              currPath = currPath->next;
              delete prev; 
            }
            return;
          }
          else {
            Folder *folder = (Folder*)folderPtr;
            SL_Node *newNode = new SL_Node(folder);
            newNode->next = currPath;
            currPath = newNode;
          }
        }
      }

      if(currFolder->folderPtr != currPath->folderPtr) {
        while(currFolder != nullptr) {
          SL_Node *prev = currFolder; 
          currFolder = currFolder->next; 
          delete prev;
        }
        currFolder = currPath;
      }
    }

    FS_Node* search(SL_Node *currFdPtr, std::string name) {
      FS_Node* ptr = currFdPtr->folderPtr->subFoldersPtr;

      while(ptr != nullptr) {
        if(ptr->name == name) break;

        if(name > ptr->name) {
          ptr = ptr->right;
        }
        else {
          ptr = ptr->left;
        }
      } 

      return ptr;
    }

    std::vector<std::string> parsePath(std::string path) {
      std::vector<std::string> folders; 
      int i=-1, j=0, n=path.size();
      std::string folderName = "";

      if(path[0] == '/') {
        j=1; 
        folders.push_back("/");
      }

      while(j<n) {
        if(path[j] == '/') {
          j++;
          continue;
        }
        i=j; 

        while(j<n && path[j] != '/') j++; 

        if(j<n) {
          folderName = ""; 
          for(;i<j;i++) {
            folderName += path[i];
          }

          if(folderName != ".") {
            folders.push_back(folderName);
          } 

          j++;
        }

      }

      if(path[n-1] != '/') {
        folderName = "";
        for(;i<n;i++) {
          folderName += path[i];
        }

        if(folderName != ".") {
          folders.push_back(folderName);
        }
      }

      return folders;
    }

    void moreSimplify(std::vector<std::string> &path) {
      for(int i=0;i<path.size();) {
        if(i>0 && path[i] == "..") {
          if(path[i-1] == "/") {
            path.erase(path.begin()+i);
          } 
          else if(path[i-1] != "..") {
            path.erase(path.begin()+i-1);
            path.erase(path.begin()+i-1);
            i=i-1;
          }
          else {
            i++;
          }
        }
        else {
          i++;
        }
      }
    }

    void cd(std::string path) {
      std::vector<std::string> folders = parsePath(path);
      moreSimplify(folders);
      changeDirectory(folders);
    }
};  
