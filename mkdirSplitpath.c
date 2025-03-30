/*/
 *   Mark Ardrey
 *   CSCI210 :: Systems Programming 
 *   Tolga Can
 *   Program to split the path fed into it into a base name, and directory path.
 *   The second part allocates memory to build a directory for use.
 *   Intended to be used with preprogrammed touch, rm, rmdir, ls, cd, pwd commands
/*/

#include "types.h"

extern struct NODE* root;
extern struct NODE* cwd;

//make directory
void mkdir(char pathName[]){

    //if the provided path name is "/" then there is no valid path
    if(strcmp(pathName, "/") == 0)
    {
        printf("MKDIR ERROR: no path provided\n");
        return;
    }

    char baseName[64];
    char dirName[256];

    struct NODE* parent = splitPath(pathName, baseName, dirName);
    if (parent == NULL)
    {
        return;
    }

    struct NODE* temp = parent->childPtr;
    int exists = 0;
    while (temp != NULL)
    {
        if(strcmp(temp->name, baseName) == 0 && temp->fileType == 'D')
        {
            exists = 1;
            break;
        }
        temp = temp->siblingPtr;
    }

    //prints error messages for incorrect inputs
    if (exists)
    {
        if (dirName[0] == '/' && dirName[1] == '\0')
        {
            printf("MKDIR ERROR: directory %s already exists\n", baseName);
        }
        else if (dirName[0] == '\0') 
        {
            printf("MKDIR ERROR: directory %s already exists\n", baseName);
        }
        else
        {
            printf("MKDIR ERROR: directory %s/%s already exists\n", dirName, baseName);
        }
    }

    //initializes new directory node using malloc
    struct NODE* newNode = (struct NODE*)malloc(sizeof(struct NODE));
    if (newNode == NULL) //sentinel for errors
    {
        perror("malloc");
        exit(1);
    }

    //builds links to other nodes for the new directory
    strncpy(newNode->name, baseName, 63);
    newNode->name[63] = '\0';
    newNode->fileType = 'D';
    newNode->childPtr = NULL;
    newNode->siblingPtr = NULL;
    newNode->parentPtr = parent;

    //creates new node as the last child
    if (parent->childPtr == NULL)
    {
        parent->childPtr = newNode;
    }
    else
    {
        struct NODE* initNode = parent->childPtr;
        while(initNode->siblingPtr != NULL)
        {
            initNode = initNode->siblingPtr;
        }
        initNode->siblingPtr = newNode;
    }

    //if the function works successfully, this message will be printed
    printf("MKDIR SUCCESS: node %s successfully created\n", pathName);
    return;
}

//handles tokenizing and absolute/relative pathing options
struct NODE* splitPath(char* pathName, char* baseName, char* dirName){

    char pathTemp[256];
    char dirTemp[256];

    strcpy(pathTemp, pathName);

    //locates the end of dirName
    int slashIndex = -1;
    for (int i = 0; pathTemp[i] != '\0'; i++)
    {
        if (pathTemp[i] == '/')
        {
            slashIndex = i;
        }
    }

    //splits the path into baseName and dirName, depending on what index the slash was found in
    if (slashIndex != -1)
    {
        //if no path to the current directory exists
        if (slashIndex == 0)
        {
            strcpy(dirName, "/");
            strcpy(baseName, pathTemp + 1);
        }
        //standard operation to copy pathTemp into dirTemp, baseName, and dirName
        //what is most common to happen
        else 
        {
            for (int j = 0; j < slashIndex; j++)
            {
                dirTemp[j] = pathTemp[j];
            }
            dirTemp[slashIndex] = '\0';
            strcpy(dirName, dirTemp);
            strcpy(baseName, pathTemp + slashIndex + 1);
        }
    }
    //handles edge case of no dirName, and only pathTemp exists
    else
    {
        dirName[0] = '\0';
        strcpy(baseName, pathTemp);
    }

    //finds the pointer to the parent directory of baseName
    struct NODE* currentNode = (pathName[0] == '/') ? root : cwd;
    if (strlen(dirName) == 0)
    {
        return currentNode;
    }

    //prepares dirName for tokenization
    char dirCopy[256];
    strcpy(dirCopy, dirName);
    if (strcmp(dirCopy, "/") == 0)
    {
        return root;
    }

    //tokenizes directory path using '/' delimiter 
    char* currentDirName = strtok(dirCopy, "/");
    while(currentDirName != NULL)
    {
        int found;
        struct NODE* child = currentNode->childPtr;
        while(child != NULL)
        {
            if (strcmp(child->name, currentDirName) == 0 && child->fileType == 'D')
            {
                found = 1;
                break;
            }
            child = child->siblingPtr;
        }
        if (!found)
        {
            printf("ERROR: directory %s does not exist\n", currentDirName);
            return NULL;
        }
        currentNode = child;
        currentDirName = strtok(NULL, "/");
    }

    return currentNode;
}
