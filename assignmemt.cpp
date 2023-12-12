#include <iostream>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <fstream>

using namespace std;

class InMemoryFileSystem {
private:
    std::string current_path;

public:
    std::string getCurrentPath() const {
        return current_path;
    }


    void mkdir(const string& directory_name) {
        string new_directory_path = current_path + directory_name;
        if (file_system.find(new_directory_path) == file_system.end()) {
            file_system[new_directory_path] = {};
        } else {
            cout << "Directory '" << directory_name << "' already exists." << endl;
        }
    }

    void cd(const string& path) {
        if (path == "/") {
            current_path = "/";
        } else if (path == "..") {
            size_t last_slash = current_path.find_last_of('/');
            if (last_slash != string::npos) {
                current_path = current_path.substr(0, last_slash);
            }
        } else {
            string new_path = current_path + path;
            if (file_system.find(new_path) != file_system.end() && file_system[new_path].is_directory) {
                current_path = new_path;
            } else {
                cout << "Invalid path: " << path << endl;
            }
        }
    }

    void ls(const string& path = ".") {
        string target_path = current_path + path;
        if (file_system.find(target_path) != file_system.end() && file_system[target_path].is_directory) {
            for (const auto& entry : file_system[target_path].contents) {
                cout << entry << endl;
            }
        } else {
            cout << "Invalid path: " << path << endl;
        }
    }

    void touch(const string& file_name) {
        string file_path = current_path + file_name;
        if (file_system.find(file_path) == file_system.end()) {
            file_system[file_path] = {true, {}};
        } else {
            cout << "File '" << file_name << "' already exists." << endl;
        }
    }

    void cat(const string& file_name) {
        string file_path = current_path + file_name;
        if (file_system.find(file_path) != file_system.end() && !file_system[file_path].is_directory) {
            // Assuming cat on a directory is not allowed
            cout << file_path << endl;
        } else {
            cout << "Invalid file: " << file_name << endl;
        }
    }

    void echo(const string& content, const string& file_name) {
        string file_path = current_path + file_name;
        if (file_system.find(file_path) != file_system.end() && !file_system[file_path].is_directory) {
            file_system[file_path].contents = content;
        } else {
            cout << "Invalid file: " << file_name << endl;
        }
    }

    void rm(const string& path) {
        string target_path = current_path + path;
        if (file_system.find(target_path) != file_system.end()) {
            file_system.erase(target_path);
        } else {
            cout << "Invalid path: " << path << endl;
        }
    }

    void cp(const string& source, const string& destination) {
        string source_path = current_path + source;
        string destination_path = current_path + destination;
        if (file_system.find(source_path) != file_system.end()) {
            file_system[destination_path] = file_system[source_path];
        } else {
            cout << "Invalid source path: " << source << endl;
        }
    }

    void mv(const string& source, const string& destination) {
        string source_path = current_path + source;
        string destination_path = current_path + destination;
        if (file_system.find(source_path) != file_system.end()) {
            file_system[destination_path] = file_system[source_path];
            file_system.erase(source_path);
        } else {
            cout << "Invalid source path: " << source << endl;
        }
    }

    void save_state(const string& file_path) {
        ofstream outfile(file_path);
        if (outfile.is_open()) {
            outfile << current_path << endl;
            for (const auto& entry : file_system) {
                outfile << entry.first << "\t" << entry.second.is_directory << "\t" << entry.second.contents << endl;
            }
            cout << "File system state saved to " << file_path << endl;
        } else {
            cerr << "Unable to open file: " << file_path << endl;
        }
    }

    void load_state(const string& file_path) {
        ifstream infile(file_path);
        if (infile.is_open()) {
            string line;
            getline(infile, line);
            current_path = line;

            while (getline(infile, line)) {
                istringstream iss(line);
                string path, is_directory_str, contents;
                if (iss >> path >> is_directory_str >> contents) {
                    bool is_directory = (is_directory_str == "1");
                    file_system[path] = {is_directory, contents};
                }
            }
            cout << "File system state loaded from " << file_path << endl;
        } else {
            cerr << "Unable to open file: " << file_path << endl;
        }
    }

private:
    string current_path;

    struct FileSystemEntry {
        bool is_directory;
        string contents; // For file content or directory entries
    };

    unordered_map<string, FileSystemEntry> file_system;
};

int main() {
    InMemoryFileSystem file_system;
    string input;

    while (true) {
        cout << file_system.getCurrentPath() << "$ ";

        getline(cin, input);

        istringstream iss(input);
        vector<string> tokens;
        string token;

        while (iss >> token) {
            tokens.push_back(token);
        }

        if (tokens.empty()) {
            continue;
        }

        string command = tokens[0];
        tokens.erase(tokens.begin());

        if (command == "exit") {
            break;
        } else if (command == "save_state") {
            if (tokens.size() == 1) {
                file_system.save_state(tokens[0]);
            } else {
                cout << "Usage: save_state <file_path>" << endl;
            }
        } else if (command == "load_state") {
            if (tokens.size() == 1) {
                file_system.load_state(tokens[0]);
            } else {
                cout << "Usage: load_state <file_path>" << endl;
            }
        } else if (command == "mkdir") {
            if (tokens.size() == 1) {
                file_system.mkdir(tokens[0]);
            } else {
                cout << "Usage: mkdir <directory_name>" << endl;
            }
        } else if (command == "cd") {
            if (tokens.size() == 1) {
                file_system.cd(tokens[0]);
            } else {
                cout << "Usage: cd <path>" << endl;
            }
        } else if (command == "ls") {
            if (tokens.size() <= 1) {
                file_system.ls(tokens.empty() ? "." : tokens[0]);
            } else {
                cout << "Usage: ls [path]" << endl;
            }
        } else if (command == "touch") {
            if (tokens.size()                == 1) {
                file_system.touch(tokens[0]);
            } else {
                cout << "Usage: touch <file_name>" << endl;
            }
        } else if (command == "cat") {
            if (tokens.size() == 1) {
                file_system.cat(tokens[0]);
            } else {
                cout << "Usage: cat <file_name>" << endl;
            }
        } else if (command == "echo") {
            if (tokens.size() >= 2) {
                string content = tokens[0];
                tokens.erase(tokens.begin());
                string file_name = tokens[0];
                file_system.echo(content, file_name);
            } else {
                cout << "Usage: echo <content> <file_name>" << endl;
            }
        } else if (command == "rm") {
            if (tokens.size() == 1) {
                file_system.rm(tokens[0]);
            } else {
                cout << "Usage: rm <path>" << endl;
            }
        } else if (command == "cp") {
            if (tokens.size() == 2) {
                string source = tokens[0];
                string destination = tokens[1];
                file_system.cp(source, destination);
            } else {
                cout << "Usage: cp <source> <destination>" << endl;
            }
        } else if (command == "mv") {
            if (tokens.size() == 2) {
                string source = tokens[0];
                string destination = tokens[1];
                file_system.mv(source, destination);
            } else {
                cout << "Usage: mv <source> <destination>" << endl;
            }
        } else {
            cout << "Invalid command: " << command << endl;
        }
    }

    cout << "Exiting the file system." << endl;

    return 0;
}

