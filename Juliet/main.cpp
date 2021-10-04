#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <filesystem>

using namespace std;

int main(){
    string line, cmd, dir, curDir="/";


    while(true){

        cout << "\nuser@pc: "<< curDir << " $> ";

        getline(cin, line);
        stringstream ss(line);
        ss >> cmd;



        // if(cmd.compare("FORMAT") == 0){
        //     // cria um novo block device
        //     curDir="/devi/sda";

        //         cout << "Formatting successfully!" << endl;
                // while(true){
                    if(cmd.compare("MKFS")==0){
                        // criar boot sector
                        // criar root directory
                        // criar FAT

                        cout << "Creating new file system successfully!" << endl;

                        if(cmd.compare("CD")==0){
                            cout << "Change Directory" << endl;
                            ss >> dir;
                            cout << dir << endl;

                        } else if (cmd.compare("DIR")==0){
                            cout << "Show Directory" << endl;

                        } else if (cmd.compare("RM")==0){
                            cout << "Remove Directory/File" << endl;

                        } else if (cmd.compare("MKDIR")==0){
                            cout << "Make Directory" << endl;

                        } else if (cmd.compare("MKFILE")==0){
                            cout << "Make File" << endl;

                        } else if (cmd.compare("EDIT")==0){
                            cout << "Edit File" << endl;

                        } else if (cmd.compare("RENAME")==0){
                            cout << "Rename Directory/File" << endl;

                        } else {
                            cout << "\nuser@pc: "<< curDir << " $> command not found: " << cmd << endl;
                        }
                    
                    } else if (cmd.compare("MOUNT")==0){

                    }/* else {
                        cout << "Could not create root directory," << endl;
                        cout << "please, delete all \"root\" directories before." << endl;
                        cout << "If the problem persist, contact your system administrator." << endl;
                    }*/

                // }
        //     } 
        // } else {
        //     cout << "\nuser@pc: "<< curDir << " $> command not found: " << cmd << endl;
        // }
    }

    return 0;
}