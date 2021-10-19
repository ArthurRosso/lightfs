#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <cstdlib>

#include "../Romeo/filesystem.h"

using namespace std;

int main(){
    string line, cmd, item, curDir="/root"; // quando criamos um novo sistema de arquivos estamos no root
    uint8_t* cache; // chamamos de cache nossa própria abstração da tabela fat como uma lista de listas de indexes

    uint8_t* dir;

    Filesystem_t* filesys = (Filesystem_t*)malloc(sizeof(Filesystem_t));
    
    open_disk(filesys, "cdoispontos.bin");

    while(true){

        cout << "\nuser@pc: /devi/sda $> ";

        getline(cin, line);
        stringstream ss(line);
        ss >> cmd;

        if(cmd.compare("MKFS")==0){
            make_filesystem(filesys);

            cout << "Creating new file system successfully!" << endl;
            break;
        
        } else if (cmd.compare("MOUNT")==0){
            mount_filesystem(filesys);

            cout << "Mounting file system successfully!" << endl;
            break;

        } else {
            cout << "\nuser@pc: /devi/sda $> you must to make or mount the filesystem first" << endl;
        }
    }

    // aqui populamos a cache, ela vai nos ajudar a falar com o romeozão
    show_dir(filesys, 0, &cache);


    while(true){

        cout << "\nuser@pc: "<< curDir << " $> ";

        getline(cin, line);
        stringstream ss(line);
        ss >> cmd;

        if(cmd.compare("CD")==0){

            // curDir.clear();
            while(getline(ss, item, '/')) {
                
                for(uint8_t i=0; i< (show_dir(filesys, 2, &dir) ); i++){

                    if(is_dir(filesys, dir[i]) && item.compare(return_name(filesys, dir[i])) == 0){
                        curDir.append("/");
                        curDir.append(item);
                    }// else { break e informar erro}

                }               
            }

        } else if (cmd.compare("DIR")==0){
            cout << "Show Directory" << endl;

        } else if (cmd.compare("RM")==0){
            cout << "Remove Directory/File" << endl;

        } else if (cmd.compare("MKDIR")==0){
            cout << "Make Directory" << endl;

        } else if (cmd.compare("MKFILE")==0){
            make_file(filesys, "teste.txt", 0, 1);
            char* texto1 = "Testando, colocando dados no cluster 1";
            write_file(filesys, 1, texto1, strlen(texto1));
            
            make_file(filesys, "Pasta", 0, 0);

        } else if (cmd.compare("EDIT")==0){
            cout << "Edit File" << endl;

        } else if (cmd.compare("RENAME")==0){
            cout << "Rename Directory/File" << endl;

        } else {
            cout << "\nuser@pc: "<< curDir << " $> command not found: " << cmd << endl;
        }
    }

    return 0;
}