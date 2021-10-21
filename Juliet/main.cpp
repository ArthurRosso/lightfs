#include <iostream>
#include <stdio.h>
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
    uint8_t current_index = 0;
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
            uint8_t index = 0;
            int count=0;
            // curDir.clear();
            //ss >> item;
            while(getline(ss, item, '/')) {
                /*
                if(index == 0 && item.compare("root") == 0){
                    curDir = "/root";
                    current_index = 0;
                    break;
                }*/

                for(uint8_t i=0; i< (show_dir(filesys, index, &dir) ); i++){

                    if(is_dir(filesys, dir[i]) && item.compare(return_name(filesys, dir[i])) == 0){
                        curDir.append("/");
                        curDir.append(item);
                        index = dir[i];
                        current_index = dir[i];
                        break;
                    }// else { break e informar erro}
                
                    
                }               
            }

        } else if (cmd.compare("DIR")==0){
            for(uint8_t i=0; i< (show_dir(filesys, current_index, &dir)); i++){
                cout << return_name(filesys, dir[i]) << "   " /*createion time*/ << endl;
            }

        } else if (cmd.compare("RM")==0){
            ss >> item;
            for(uint8_t i=0; i< (show_dir(filesys, current_index, &dir) ); i++){
                if(item.compare(return_name(filesys, dir[i])) == 0){                       
                    if(is_dir(filesys, dir[i]) && child_num(filesys, dir[i]) != 0){
                        break; // mensagem caso tenha pasta e ela tem coisa
                    }
                    delete_file(filesys, dir[i], current_index);
                }
                // mensagem caso nao tenha o arquivo desejado
            }

        } else if (cmd.compare("MKDIR")==0){
            // Colisão de nome
            make_file(filesys, "Pasta", current_index, 0);


        } else if (cmd.compare("MKFILE")==0){
            // Colisão de nome
            make_file(filesys, "teste.txt", current_index, 1);
            //char* texto1 = "Testando, colocando dados no cluster 1";
            //write_file(filesys, 1, texto1, strlen(texto1));
            
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