#include<iostream>
#include <vector>
#include<string>
#include<sstream>
#include <ctime>
using namespace std ;

class TreeNode{

    public:
    int version_id;
    string content;
    string message;
    time_t created_timestamp;
    time_t snapshot_timestamp;
    TreeNode* parent;
    vector<TreeNode*> children;

    TreeNode(int id, string text = "", string msg = ""){
        version_id = id;
        content = text;
        message = msg;
        parent = nullptr;
        created_timestamp = time(0);
        snapshot_timestamp = 0;
    }

};

class MyMap_for_trees {
    private:
        class Holder{
            public:
            int key;
            TreeNode* value;
            bool occupied;
            Holder(){
                key = -1;
                value = nullptr;
                occupied = false;
            }
        };

        vector<Holder> table;
        int capacity;
        int size;

        int hashvalue(int key){
            return key % capacity;
        }

        void rehash(){
            int old_capacity = capacity;
            vector<Holder> old_table = table;

            capacity *= 2;
            table.clear();
            table.resize(capacity);
            size = 0;

            for (int i = 0; i < old_capacity; i++){
                if (old_table[i].occupied) {
                    add(old_table[i].key, old_table[i].value);
                }
            }
        }

    public:
        MyMap_for_trees(int initial_capacity = 16){
            capacity = initial_capacity;
            table.resize(capacity);
            size = 0;
        }

        void add(int version_id, TreeNode* node){
            if (size * 2 >= capacity) {
                rehash();
            }

            int index = hashvalue(version_id);
            while (table[index].occupied && table[index].key != version_id){
                index = (index + 1) % capacity;
            }
            if (!table[index].occupied){
                size++;
            }

            table[index].key = version_id;
            table[index].value = node;
            table[index].occupied = true;
        }

        TreeNode* search(int version_id){
            int index = hashvalue(version_id);
            int start = index;

            while (table[index].occupied){
                if (table[index].key == version_id){
                    return table[index].value;
                }
                index = (index + 1) % capacity;
                if (index == start){
                    break;
                }
            }
            return nullptr;
        }
};

class File{
    private:
    TreeNode* root;
    TreeNode* active_version;
    MyMap_for_trees version_map;
    int total_versions;

    public:

    string name;
    time_t last_modified;

    File(string fname = ""){
        name = fname;
        root = nullptr;
        active_version = nullptr;
        total_versions = 0;
        last_modified = time(0);
    }

    ~File( ){
        deleteTree(root);
    }

    string Safe_content_preview( string& s ){
        for (int i = 0; i < s.length(); i++) {
            if (!isspace((unsigned char)s[i])){
                return s;
            }
        }
        return "[empty content]";
    }

    void deleteTree(TreeNode* node){
        if (node == nullptr){
            return;
        }
        for (int i = 0; i < node->children.size(); i++){
            deleteTree(node->children[i]);
        }
        delete node;
    }

    void updateLastModified(){
        last_modified = time(0); 
    }

    int getVersionCount(){
        return total_versions;
    }

    void CREATE( ){
        root = new TreeNode(0 , "" , "Initial snapshot , i.e. root snapshot");
        version_map.add( 0 , root );
        total_versions = 1;
        active_version = root;
        root -> snapshot_timestamp = time(0);
        updateLastModified();
    }

    void READ( ){
        cout << Safe_content_preview(active_version->content) << endl;
    }

    void INSERT( string content , string& filename ){
        if( active_version -> snapshot_timestamp != 0){
            TreeNode* temp = new TreeNode( total_versions , (active_version -> content) + content );
            temp -> parent = active_version;
            (active_version -> children).push_back( temp ) ;
            active_version = temp;
            version_map.add(total_versions, temp);
            total_versions += 1;
        }
        else{
            (active_version -> content) = (active_version -> content) + content ;
        }
        cout << Safe_content_preview( content ) << " successfully inserted in " << filename << endl;
        updateLastModified();
    }

    void UPDATE( string content , string& filename ){

        if( active_version -> snapshot_timestamp != 0){
            TreeNode* temp = new TreeNode( total_versions , content );
            temp -> parent = active_version;
            (active_version -> children).push_back( temp ) ;
            active_version = temp;
            version_map.add(total_versions, temp);
            total_versions += 1;
        }
        else{
            active_version -> content = content;
        }
        cout << Safe_content_preview( content ) << " successfully inserted in " << filename << endl;
        updateLastModified();
    }

    void SNAPSHOT( string message , string& filename ){

        if (active_version->snapshot_timestamp != 0){ 
            cout << "Already a snapshot" << endl; 
            return;
        }

        active_version -> snapshot_timestamp = time(0);
        active_version -> message = message;
        cout << Safe_content_preview(message) << " successfully made a snapshot in " << filename << endl;
        updateLastModified();
    }

    void ROLLBACK( string& filename ,int version_id = -1 ){

        if( version_id == -1 ){
            if( active_version == nullptr ){
                cout << "Cannot rollback: active version is null" << endl;
                return;
            }
            if (active_version->parent != nullptr){
                active_version = active_version->parent;
                cout << "Successfully rolled back '" << filename << "'." << endl;
            }
            else{
                cout << "Already at root, cannot rollback to parent" << endl;
            }
        }
        else{
            if( version_id >= 0 && version_id < total_versions ){
                TreeNode* node = version_map.search(version_id);
                if (node != nullptr){
                    active_version = node;
                    cout << "Successfully rolled back '" << filename << "'." << endl;
                }   
                else{
                    cout << "Version " << version_id << " not found in version_map" << endl;
            }
            }
            else{
                cout << "Pls provide a valid id, your id is either less than 0 or not available" << endl;
            }
        }
        updateLastModified();
    }

    void HISTORY( ){
        vector<TreeNode*> snapshots;
        TreeNode* temp = active_version;
        while (temp != nullptr){
            if (temp->snapshot_timestamp != 0){
                snapshots.push_back(temp);
            }
            temp = temp->parent;
        }
        int n = snapshots.size();
        for (int i = n- 1; i >= 0; i--){
            TreeNode* snaps = snapshots[i];
            cout << "Version : " << snaps->version_id << " ," << " timestamp : " << snaps->snapshot_timestamp << " ," << " message : " << Safe_content_preview(snaps->message) << "." << endl;
        }
    }
};

class MyMap_for_files {
    private:
        class Holder{
            public:
            string key;
            File* value;
            bool occupied;
            Holder(){
                key = "";
                value = nullptr;
                occupied = false;
            }
        };
    
        vector<Holder> table;
        int capacity;
        int size;
    
        int hashvalue( string& key){
            long hash = 0;
            long p = 31;
            long m = 1e9 + 9;
    
            for (int i = 0; i < key.length(); ++i){
                hash = (hash * p + key[i]) % m;
            }

            return (int)(hash % capacity);
        }
    
        void rehash(){
            int old_capacity = capacity;
            vector<Holder> old_table = table;
    
            capacity *= 2;
            table.clear();
            table.resize(capacity);
            size = 0;
    
            for (int i = 0; i < old_capacity; i++){
                if (old_table[i].occupied){
                    add(old_table[i].key, old_table[i].value);
                }
            }
        }
    
    public:
        MyMap_for_files(int initial_capacity = 16){
            capacity = initial_capacity;
            table.resize(capacity);
            size = 0;
        }
    
        void add( string& filename, File* f){
            if (size * 2 >= capacity) {
                rehash();
            }
    
            int index = hashvalue(filename);
            while (table[index].occupied && table[index].key != filename){
                index = (index + 1) % capacity;
            }
            if (!table[index].occupied){
                size++;
            }
    
            table[index].key = filename;
            table[index].value = f;
            table[index].occupied = true;
        }
    
        File* search( string& filename ){
            int index = hashvalue(filename);
            int start = index;
    
            while ( table[index].occupied ){
                if ( table[index].key == filename ) {
                    return table[index].value;
                }
                index = (index + 1) % capacity;
                if ( index == start ){
                    break;
                }
            }
            return nullptr;
        }
    };

class MaxHeap_for_recent {
private:
    vector<File*> heap;

    void Heapify_up(int i){
        while (i > 0){
            int parent = (i - 1) / 2;
            if ( heap[parent]->last_modified >= heap[i]->last_modified ){
                break;
            }
            swap(heap[parent], heap[i]);
            i = parent;
        }
    }

    void Heapify_down(int i){
        int n = heap.size();
        while (true){
            int left = 2 * i + 1, right = 2 * i + 2, largest = i;
            if ( left < n && heap[left]->last_modified > heap[largest]->last_modified ){
                largest = left;
            }
            if ( right < n && heap[right]->last_modified > heap[largest]->last_modified ){
                largest = right;
            }
            if ( largest == i ){
                break;
            }
            swap(heap[i], heap[largest]);
            i = largest;
        }
    }

public:
    void push(File* f){
        heap.push_back(f);
        Heapify_up(heap.size() - 1);
    }

    File* pop(){
        if (heap.empty()){
            return nullptr;
        }
        File* top = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        Heapify_down(0);
        return top;
    }

    bool empty(){
        return heap.empty();
    }
};

class MaxHeap_for_biggest {
private:
    vector<File*> heap;

    void Heapify_up(int i){
        while (i > 0){
            int parent = (i - 1) / 2;
            if ( heap[parent]->getVersionCount() >= heap[i]->getVersionCount() ){
                break;
            } 
            File* tmp = heap[parent];
            heap[parent] = heap[i];
            heap[i] = tmp;
            i = parent;
        }
    }

    void Heapify_down(int i){
        int n = heap.size();
        while (true){
            int left = 2 * i + 1, right = 2 * i + 2, largest = i;
            if ( left < n && heap[left]->getVersionCount() > heap[largest]->getVersionCount() ){
                largest = left;
            }
            if ( right < n && heap[right]->getVersionCount() > heap[largest]->getVersionCount() ){
                largest = right;
            }
            if ( largest == i ){
                break;
            } 
            File* tmp = heap[i];
            heap[i] = heap[largest];
            heap[largest] = tmp;
            i = largest;
        }
    }

public:
    void push(File* f){
        heap.push_back(f);
        Heapify_up(heap.size() - 1);
    }

    File* pop(){
        if ( heap.empty() ){
            return nullptr;
        }
        File* top = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if ( !heap.empty() ){
            Heapify_down(0);
        }
        return top;
    }

    bool empty(){
        return heap.empty();
    }
};

class FileSystem {
private:
    vector<File*> files;
    MyMap_for_files file_map;

    File* Find_file( string& filename ){
        return file_map.search(filename); 
    }

    bool Compute_Recent( File* a, File* b ){
        return a->last_modified > b->last_modified;
    }

    bool Compute_Biggest( File* a, File* b ){
        return a->getVersionCount() > b->getVersionCount();
    }

public:
    void Create_file( string& filename ){
        if ( Find_file(filename) ){
            cout << "File already exists" << endl;
            return;
        }
        File* f = new File(filename);
        string temp = filename;
        f->CREATE( );
        files.push_back(f);
        file_map.add(filename, f); 
        
        cout << "File " << filename << " has been successfully created ." << endl;
    }

    File* Get_file( string& filename ){
        File* f = Find_file(filename);
        if (!f){
            cout << "File not found" << endl;
        }
        return f;
    }

    void Recent_files(int num){
        MaxHeap_for_recent temp;
        for ( int i = 0; i < files.size(); i++ ){
            temp.push(files[i]);
        }
        if( files.size() == 0 ){
            cout << "No File has been created, first create a file. " << endl;
            return;
        }
        for ( int i = 0; i < num && !temp.empty(); i++ ){
            File* f = temp.pop();
            string time_str = ctime(&(f->last_modified));
            time_str.erase(time_str.length() - 1);
            cout << f->name << " ( " << "last modified: " << time_str << " )" << endl;
        }
    }

    void Biggest_trees(int num){
        MaxHeap_for_biggest temp;
        for( size_t i = 0; i < files.size(); i++ ){
            temp.push(files[i]);
        }
        if( files.size() == 0 ){
            cout << "No File has been created, first create a file. " << endl;
            return;
        }
        for( int i = 0; i < num && !temp.empty(); i++ ){
            File* f = temp.pop();
            cout << f->name << " ( " << "versions: " << f->getVersionCount() << " )" << endl;
        }
    }

    ~FileSystem(){
        for ( size_t i = 0; i < files.size(); i++ ){
            delete files[i];
        }
    }
};

int main(){

    FileSystem fs;

    while( true ){
        string input , operation, filename , rest , anything_extra;
        cout << endl;
        cout << "Enter the command you want to perform or EXIT if you want to end the programme. " << endl;
        cout << endl;

        getline(cin, input);
        stringstream ss(input);

        ss >> operation;
        if ( !( operation == "CREATE" || operation == "READ" || operation == "INSERT" || operation == "UPDATE" || operation == "SNAPSHOT" || operation == "ROLLBACK" || operation == "HISTORY" || operation == "RECENT_FILES" || operation == "BIGGEST_TREES" || operation == "EXIT" ) ){
            cout << "Incorrect operation given, please choose : CREATE , READ , INSERT, UPDATE , SNAPSHOT , ROLLBACK , HISTORY , RECENT_FILES , BIGGEST_TREES , EXIT " << endl;
            continue;
        }

        if( operation == "EXIT" ){
            break;
        }

        if( operation == "RECENT_FILES" ){
            ss >> rest;
            try{
                string s = rest;
                size_t pos;
                int num = stoi(s, &pos);

                if ( pos != s.size() ){
                    cout << "Error: invalid characters after number" << endl;
                    continue;
                }
                if( (ss >> anything_extra) ){
                    cout << "Too many arguments given, pls give command in RECENT_FILES <num> format." << endl;
                    continue;
                }
                if ( num <= -1 ){
                    cout << "Error: Please provide a valid num." << endl;
                    continue;
                }
                fs.Recent_files(num);
                continue;
            }
            catch (...){
                cout << "Error: Please provide a valid num." << endl;
                continue;
            }
        }

        if( operation == "BIGGEST_TREES"){
            ss >> rest;
            try{
                string s = rest;
                size_t pos;
                int num = stoi(s, &pos);

                if ( pos != s.size() ){
                    cout << "Error: invalid characters after number" << endl;
                    continue;
                }
                if( ( ss >> anything_extra) ){
                    cout << "Too many arguments given, pls give command in BIGGEST_TREES <num> format." << endl;
                    continue;
                }
                if ( num <= -1 ){
                    cout << "Error: Please provide a valid number." << endl;
                    continue;
                }
                fs.Biggest_trees(num);
            }
            catch (...){
                cout << "Error: Please provide a valid num." << endl;
                continue;
            }
        }

        if ( !(ss >> filename) && !(operation == "BIGGEST_TREES") && !(operation == "RECENT_FILES") ){
            cout << "Insufficient information for name of file is given , pls write the <filename> after the operation. " << endl;
            continue;
        }
    
        getline(ss, rest);
        if ( !rest.empty() && rest[0] == ' ' ){
            rest.erase(0, 1);
        }

        if( operation == "CREATE" ){
            if( rest != "" ){
                cout << "Wrong syntax given, it should be CREATE <filename>." << endl;
                continue;
            }
            else{
                fs.Create_file( filename );
                continue;
            }
        }
        else if( operation == "READ" ){
            if( rest != "" ){
                cout << "Wrong syntax given, it should be READ <filename>." << endl;
                continue;
            }
            File* f = fs.Get_file(filename);
            if (f){
                f->READ( );
            }
            continue;
        }
        else if( operation == "INSERT" ){
            if ( rest.empty() ){ 
                cout << "Missing content/message." << endl; 
                continue; 
            }
            File* f = fs.Get_file(filename);
            if (f){
                f->INSERT( rest , filename );
                continue;
            }
        }
        else if( operation == "UPDATE" ){
            if ( rest.empty() ){ 
                cout << "Missing content/message." << endl;
                continue; 
            }
            File* f = fs.Get_file(filename);
            if (f){
                f->UPDATE( rest , filename );
                continue;
            }
        }
        else if( operation == "SNAPSHOT" ){
            if ( rest.empty() ){ 
                cout << "Missing content/message." << endl; 
                continue; 
            }
            File* f = fs.Get_file(filename);
            if (f){
                f->SNAPSHOT( rest , filename );
                continue;
            }
        }
        else if( operation == "ROLLBACK"){
            File* f = fs.Get_file(filename);
            if (f){
                int id = -1;
                if (!rest.empty()){
                    try{
                        string s = rest;
                        size_t pos;
                        int num = stoi(s, &pos);

                        if ( pos != s.size() ){
                            cout << "Error: invalid characters after number" << endl;
                            continue;
                        }
                        else{
                            id = num;
                        }
                    }
                    catch (...){
                        cout << "Error: Please provide a valid version ID to rollback to, or no ID to rollback to parent." << endl;
                        continue;
                    }
                }
                f->ROLLBACK( filename, id );
                continue;
            }
        }
        else if( operation == "HISTORY" ){
            if( rest != "" ){
                cout << "Wrong syntax given, it should be HISTORY <filename>." << endl;
                continue;
            }
            else{
                File* f = fs.Get_file( filename );
                if (f){
                    cout <<  "Here is the history of " << filename << endl;
                    f->HISTORY( );
                    continue;
                }
            }
        }
    }
    return 0;
};