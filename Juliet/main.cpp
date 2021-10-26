#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <time.h>
#include <algorithm>

#include "../Romeo/filesystem.h"

using namespace std;

int main(){
    string line, cmd, item, curDir="/root"; // quando criamos um novo sistema de arquivos estamos no root
    uint8_t* cache; // chamamos de cache nossa própria abstração da tabela fat como uma lista de listas de indexes
    uint8_t current_index = 0;
    uint8_t* dir;
    char name[23];

    Filesystem_t* filesys = (Filesystem_t*)malloc(sizeof(Filesystem_t));
    
    open_disk(filesys, "cdoispontos.bin");

    while(true){

        cout << "\nuser@pc: /devi/sda $> ";

        getline(cin, line);
        stringstream ss(line);
        ss >> cmd;
        for_each(cmd.begin(), cmd.end(), [](char & c){
            c =toupper(c); 
        });

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
        for_each(cmd.begin(), cmd.end(), [](char & c){
            c =toupper(c); 
        });
    
        if(cmd.compare("CD")==0){
            bool entred = false;
            bool changed;
            uint8_t index = 0;
            int count=0;
            string new_dir = "/root";

            getline(ss, item, '/');
            getline(ss, item, '/');
            if(item.compare("root") != 0){
                cout << "The path must be in /root!" << endl;
            }
            else{ 
                changed = true;
                while(getline(ss, item, '/')) {
                    entred = false;
                    for(uint8_t i=0; i<(show_dir(filesys, index, &dir)); i++){
                        if(is_dir(filesys, dir[i]) == true && item.compare(return_name(filesys, dir[i])) == 0){
                            new_dir.append("/");
                            new_dir.append(item);
                            index = dir[i];
                            current_index = dir[i];
                            entred = true;
                            count++;
                            break;
                        }              
                    }
                    if(entred == false){
                        cout << item << " not found!" << endl;
                        changed = false;
                    }

                }
                if(changed)
                    curDir = new_dir;
                if(changed && count == 0)
                    current_index = 0;

            }


        } else if (cmd.compare("DIR")==0){
            struct tm  ts;
            char buftime[80];
            if(show_dir(filesys, current_index, &dir) == 0)
                cout << "Diretory empty!" << endl;
            for(uint8_t i=0; i< (show_dir(filesys, current_index, &dir)); i++){
                time_t time = return_time(filesys, dir[i]);
                ts = *localtime(&time);
                strftime(buftime, sizeof(buftime), "%a %Y-%m-%d  %H:%M:%S | ", &ts);
                cout << buftime << return_name(filesys, dir[i])  << endl;
            }

        } else if (cmd.compare("RM")==0){
            ss >> item;
            bool found = false;
            for(uint8_t i=0; i< (show_dir(filesys, current_index, &dir) ); i++){
                if(item.compare(return_name(filesys, dir[i])) == 0){  
                    found = true;                     
                    if(is_dir(filesys, dir[i]) && child_num(filesys, dir[i]) != 0){
                        cout << return_name(filesys, dir[i]) << " is a non empty directory." << endl;
                        break; 
                    }
                    delete_file(filesys, dir[i], current_index);
                }
                
            }
            if(found == false)
                cout << item << " not found." << endl;

        } else if (cmd.compare("MKDIR")==0){
            bool create = true;
            ss >> item;
            strcpy(name, item.c_str());
            if(strlen(name) == 0){
                cout << "Name may not be blank." << endl;
            }
            else if (strlen(name) > 23) {
                cout << "Name may not be longer than 23 characteres!" << endl;
            }
            else{
                uint8_t index = 0;
                for(uint8_t i=0; i< (show_dir(filesys, current_index, &dir) ); i++){
                    if(item.compare(return_name(filesys, dir[i])) == 0){     
                        create = false;                  
                        cout << "This name already exist!" << endl;    
                        break;
                    }
                }            
                if(create)
                    make_file(filesys, name, current_index, 0);
            }

        } else if (cmd.compare("MKFILE")==0){
            bool create = true;
            ss >> item;
            strcpy(name, item.c_str());
            if(strlen(name) == 0){
                cout << "Name may not be blank." << endl;
            }
            else if (strlen(name) > 23) {
                cout << "Name may not be longer than 23 characteres!" << endl;
            }
            else{
                int8_t index = 0;
                for(uint8_t i=0; i< (show_dir(filesys, current_index, &dir) ); i++){
                    if(item.compare(return_name(filesys, dir[i])) == 0){     
                        create = false;                  
                        cout << "This name already exist!" << endl;    
                        break;
                    }
                }            
                if(create == true)
                    make_file(filesys, name, current_index, 1);
            }
            
        } else if (cmd.compare("EDIT")==0){
            ss >> item;
            string c;
            bool mod = false;
            for(uint8_t i=0; i< (show_dir(filesys, current_index, &dir) ); i++){
                if(item.compare(return_name(filesys, dir[i])) == 0){
                    while(getline(ss, item, '\"')){
                        c.append(item);
                    }
                    int tam = c.length();
                    char content[tam]; 
                    strcpy(content, c.c_str());
                    write_file(filesys, dir[i], content, strlen(content));
                    mod = true;
                }
            }
            if(mod == false)
                cout << item << " not found." << endl;

        } else if (cmd.compare("RENAME")==0){
            bool changed = false;
            bool exist = false;
            string new_name;
            ss >> item;
            ss >> new_name;
            uint8_t index = 0;
            for(uint8_t i=0; i< (show_dir(filesys, current_index, &dir) ); i++){
                if(new_name.compare(return_name(filesys, dir[i])) == 0){
                    exist = true;
                }
            }
            if(!exist){
                for(uint8_t i=0; i< (show_dir(filesys, current_index, &dir) ); i++){
                    if(item.compare(return_name(filesys, dir[i])) == 0){
                        strcpy(name, new_name.c_str());
                        set_name(filesys, dir[i], name);
                        changed = true;
                        break;
                    }
                }
                if(!changed){
                    cout << item <<" not found." << endl;  
                }
            }
            else{
                cout << new_name << " already exist." << endl;
            }                   

        } 
        else if (cmd.compare("MOVE")==0){
            uint8_t father=0, src=0, dst=0;
            string f1, f2;
            ss >> f1;
            ss >> f2;
            stringstream fi1, fi2;
            fi1 << f1;
            fi2 << f2;
            while(getline(fi1, item, '/')) {
                father=src;
                cout << "item1 " << item << endl;
                for(int i=0; i< (show_dir(filesys, src, &dir) ); i++){
                    if(is_dir(filesys, dir[i]) && item.compare(return_name(filesys, dir[i])) == 0){
                        src = dir[i];
                        continue;
                    }
                }
            }
            while(getline(fi2, item, '/')) {
                cout << "item2 " << item << endl;
                for(int i=0; i< (show_dir(filesys, dst, &dir) ); i++){
                    if(is_dir(filesys, dir[i]) && item.compare(return_name(filesys, dir[i])) == 0){
                        dst = dir[i];
                        continue;
                    }
                }
            }

            change_child(filesys, src, dst, father);



        }else {
            cout << "\nuser@pc: "<< curDir << " $> command not found: " << cmd << endl;
        }
    }

    return 0;
}