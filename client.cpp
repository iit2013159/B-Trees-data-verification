#include <bits/stdc++.h>
#include "sha256.h"
using namespace std;

// A BTree node
class BTreeNode
{
public:
    int *keys;
    int t,n; //n is number of keys and t is order
    BTreeNode **C, *next, *parent; 
    bool leaf; 
    string hash;
    string *data;
    BTreeNode(int _t, bool _leaf){ // Constructor
        t = _t;
        leaf = _leaf;
        keys = new int[t];
        data = new string[t];
        C = new BTreeNode *[t+1];
        n = 0;
        //data ='\0';
        next = NULL;
        parent = NULL;
        hash ='\0';
    };
};
string decrypt(int n, int d, string msg);

BTreeNode *parentnotfull(BTreeNode *ptr, int k, BTreeNode *z);
BTreeNode *update_parent(BTreeNode *ptr, int k);
BTreeNode *search(BTreeNode *root, int k){
    if(root->leaf == true) {
        //cout << "leaf " << root->keys[0] << endl; 
        return root;
    }
    
    int start = 0;
    for(int i = 0; i < root->n ; i++){
        if(k < root->keys[i]) {
                //cout << "break" << root->keys[i] << " ";
                break;
        }else {
            //cout << root->keys[i] << " ";
            start++;
        }
    }
    return search(root->C[start],k);
}


BTreeNode *insertNonFull(auto k, BTreeNode *ptr){
    int i = ptr->n-1;
    //cout << "k[0] " << k[0] << " ";
    while (i >= 0 && ptr->keys[i] > stoi(k[0])){
        ptr->keys[i+1] = ptr->keys[i];
        //cout << "shifting " << ptr->data[i] << " ";
        ptr->data[i+1] = ptr->data[i];
        i--;
    }

    //cout << "i + 1 " << i+1 << endl;
    ptr->keys[i+1] = stoi(k[0]);
    ptr->data[i+1] = ' ';
    for(auto j = 1; j < k.size();j++){
            ptr->data[i+1].append(" ");
            ptr->data[i+1].append(k[j]);
        }
    ptr->n = ptr->n+1;
    return ptr;
}

BTreeNode *update_parent(BTreeNode *ptr, int k,BTreeNode *z){
    BTreeNode *tmp = new BTreeNode(ptr->t, false);
    vector <int>a;
    int q;
    for(q = 0; q < ptr->n; q++){
        a.push_back(ptr->keys[q]);
    }
    a.push_back(k);
    sort(a.begin(), a.end());

    for(int i = 0; i < ptr->t/2; i++){
        ptr->keys[i] = a[i];
        tmp->keys[i] = a[i+1+ptr->t/2];
    }
    int sv = a[ptr->t/2];
    a.erase (a.begin(),a.end());
    tmp->n = ptr->t/2;
    BTreeNode *par;
    if(ptr->parent != NULL){
        if(ptr->parent->n == ptr->t){
            par = update_parent(ptr->parent, sv, tmp);  
        } else{ 
            //cout << "parent not full\n";
            par = parentnotfull(ptr->parent, sv,tmp);
        }
        //return par;
    } else{
        //cout << "creating new root\n";
        BTreeNode *new_root = new BTreeNode(ptr->t, false);
        new_root->keys[0] = sv;
        new_root->C[0] = ptr;
        new_root->C[1] = tmp;
        new_root->n = 1;
        ptr->parent = new_root;
        tmp->parent = new_root;
        //return new_root;
    }
    if(sv == k){
        //cout << "sv = k\n";
        for(int i = ptr->t/2+1; i <= ptr->n; i++){
            tmp->C[i-ptr->t/2] = ptr->C[i];
            ptr->C[i]->parent = tmp;

        }
        tmp->C[0] = z;
        z->parent = tmp;
        ptr->n = ptr->t/2;
    }
    else if(sv > k){
        //cout << "sv > k\n";
        for (int j = ptr->t/2; j <= ptr->n; j++){
            tmp->C[j-ptr->t/2] = ptr->C[j];
            ptr->C[j]->parent = tmp;
        }

        ptr->n = ptr->t/2;
        int i = ptr->n-1;
        while(i >= 0 && ptr->keys[i] > k) i--;

        for(int j = ptr->n-1; j > i; j--){
            ptr->C[j+1] = ptr->C[j];
        }

        ptr->C[i+1] = z;
        z->parent = ptr;
    } else if(sv < k) {
        //cout << "sv < k " << endl;
        int i = tmp->n-1;
        while(i >= 0 && tmp->keys[i] > k) i--;
    
        int k=0;
        for(int j = 0; j <= tmp->n; j++){
            if(j == i+1){ 
                tmp->C[j] = z;
                z->parent = tmp;
            } else  {
                tmp->C[j] = ptr->C[ptr->t/2+1+k];
                ptr->C[ptr->t/2+1+k]->parent = tmp;
                k++;
            } 
        }
        ptr->n = ptr->t/2;
    }
    return ptr->parent;
}

void traverse(BTreeNode *root){
    if(root != NULL) {
        if(root->leaf == false){
            for (int i = 0; i < root->n+1; i++)
                traverse(root->C[i]);
        }else{
            if(root->parent!= NULL) cout << root->parent->keys[0] << ": ";
            for (int i = 0; i < root->n; i++)
                cout << root->keys[i] << " " << root->data[i] << " ";
            cout << endl;
        }
    }
}

BTreeNode *parentnotfull(BTreeNode *ptr, int k, BTreeNode *tmp){
    int i = ptr->n-1;
    //cout << "parentnotfull\n" << " " << ptr->keys[0] <<" " << i;
    //cout << "key " << k << endl;
    while(i >= 0 && ptr->keys[i] > k) {
        i--;
    }
    //cout << i << endl;
    for (int j = ptr->n; j >= i+2; j--){
        ptr->C[j+1] = ptr->C[j];
    }
     
    ptr->C[i+2] = tmp;
    tmp->parent = ptr;
    for (int j = ptr->n-1; j > i; j--){
        ptr->keys[j+1] = ptr->keys[j];
    }
    ptr->keys[i+1] = k;
    ptr->n = ptr->n+1;
    return ptr;
}


BTreeNode *splitChild (BTreeNode *ptr, auto k){
    BTreeNode *tmp = new BTreeNode(ptr->t, true);
    vector <pair <int,string > >a;
    int q;
    for(q = 0; q < ptr->n; q++){
        //cout << "ptr->data " << ptr->data[q] << " ";
        a.push_back(make_pair(ptr->keys[q],ptr->data[q]));
        //cout <<"succes " <<endl;
    }
    string sd;

    
    for(auto i = 1;i < k.size();i++){
        sd.append(" ");
        sd.append(k[i]);
    }
    a.push_back(make_pair(stoi(k[0]),sd));
    
    sort(a.begin(), a.end(),[](auto &left, auto &right) {return left.first < right.first;});
    
    for(int i = 0; i < (ptr->t/2); i++){
        ptr->keys[i] = a[i].first;
        ptr->data[i] = a[i].second;
    }

    for(int i = 0; i <= ptr->t/2; i++){
        tmp->keys[i] = a[i+ptr->t/2].first;
        tmp->data[i] = a[i+ptr->t/2].second;
    }
    
    int sv = a[ptr->t/2].first;
    a.erase (a.begin(),a.end());
    tmp->n = ptr->t/2+1;
    ptr->n = ptr->t/2;
    tmp->next = ptr->next;
    ptr->next = tmp;
    BTreeNode *par = ptr->parent;
    
    if(par != NULL){
        //cout << "pm " << par->keys[0] << endl;
        if(ptr->parent->n == ptr->t){
            BTreeNode *prnt = update_parent(ptr->parent, sv,tmp);//going in par or tmp->parent or ptr->parent and coming in par->parent         
            while(prnt->parent){
                prnt = prnt->parent;
            }
            return prnt;        
        } 
        else{ 
            //cout << "parent not full " << sv;
            par = parentnotfull(ptr->parent, sv,tmp);
            while(par->parent){
                par = par->parent;
            } 
            return par;
        }
        
    } 
    else{
        //cout << "creating new root\n";
        BTreeNode *new_root = new BTreeNode(ptr->t, false);
        new_root->keys[0] = sv;
        new_root->C[0] = ptr;
        new_root->C[1] = tmp;
        new_root->n = 1;
        ptr->parent = new_root;
        tmp->parent = new_root;
        return new_root;
    }
}

BTreeNode *insert(BTreeNode *root, auto k){
    if (root == NULL)
    {
        root = new BTreeNode(4,true);
        root->keys[0] = stoi(k[0]); 
        for(auto i = 1; i < k.size();i++){
            root->data[0].append(" ");
            root->data[0].append(k[i]);

        }
        root->n = 1; 
    }
    else { 
        BTreeNode *ptr = search(root, stoi(k[0]));
        //cout << k[0] << endl;
        if(ptr->n == ptr->t){
            ptr = splitChild(ptr,k); //returning ptr->parent
            root = ptr;
        }else{
            ptr = insertNonFull(k,ptr);
        }
        
    }
    return root;
}

string hashcompute(BTreeNode *root){
    if(root->leaf == true){
        string hs;
        for(int i = 0; i < root->n;i++){
            stringstream ss;
            ss << root->keys[i] ;
            string newstring = ss.str();
            hs.append(newstring);
        }
        root->hash = sha256(hs);
        return sha256(hs);
    }else{
        string rs;
        for(int i = 0; i <= root->n;i++){
            rs.append(hashcompute(root->C[i]));
        }
        root->hash = sha256(rs);
        return sha256(rs);
    }
}
bool searchvalue(BTreeNode *root ,int data1,int data2){
    while(root->leaf == false){
        if(root->C[0]){
            root = root->C[0];
        }
    }
    int flag = 0;
    while(root != NULL){
        for(int i = 0; i< root->n;i++){
            if(root->keys[i] >= data1 && root->keys[i] <= data2){
                flag = 1;
                cout << root->keys[i] <<" ";
            }
        }
        root = root->next;
    }
    return flag;
}
string hashrange(BTreeNode *root, BTreeNode *start,BTreeNode *end,int s,int e){ 
    if(start == end){
        if(start->leaf == false){
            string ts;
            for (int i = 0; i <= start->n; i++)
            {
                if(start->keys[i] <= e && start->keys[i] >= s ){
                    stringstream ss;
                    ss << start->C[i]->hash;
                    string newstring = ss.str();
                    ts.append(newstring);
                }
            }
            start->hash = sha256(ts);
        }
        else{
            string ts;
            for (int i = 0; i < start->n; i++)
            {
                if(start->keys[i] <= e && start->keys[i] >= s ){
                    stringstream ss;
                    ss << start->keys[i];
                    string newstring = ss.str();
                    ts.append(newstring);
                }
            }
            start->hash = sha256(ts);
        }
        return start->hash;
    }
    if(start->leaf && end->leaf){
        string hs,ts;
        for(int i = 0; i < start->n && start->keys[i] >= s;i++){
            stringstream ss;
            ss << start->keys[i] ;
            string newstring = ss.str();
            hs.append(newstring);
        }
        start->hash = sha256(hs);
        for(int i = 0; i < end->n && end->keys[i] <= e;i++){
            stringstream ss;
            ss << end->keys[i] ;
            string newstring = ss.str();
            ts.append(newstring);
        }
        end->hash = sha256(ts);
    }
    else{
        string hs,ts;
        for(int i = 0; i <= start->n && start->keys[i] >= s;i++){
            stringstream ss;
            ss << start->C[i]->hash ;
            string newstring = ss.str();
            hs.append(newstring);
        }
        start->hash = sha256(hs);
        for(int i = 0; i <= end->n && end->keys[i] <= e;i++){
            stringstream ss;
            ss << end->C[i]->hash ;
            string newstring = ss.str();
            ts.append(newstring);
        }
        end->hash = sha256(ts);
    }
    return hashrange(root,start->parent,end->parent,s,e);
}
vector<string> explode(string const & s, char delim)
{
    vector<string> result;
    istringstream iss(s);

    for (string token; getline(iss, token, delim); )
    {
        result.push_back(move(token));
    }

    return result;
}
string decrypt(int n, int d, string msg)
{
    long int pt, ct, key = d, k, m[1000];
    long int i,j,end[1000];
    for(i = 0; i < msg.size(); i++){
        end[i] = long (msg[i]);
    }
    end[i] = -1;
    i = 0, j;
    while (end[i] != -1)
    {
        ct = end[i] - 96;
        k = 1;
        for (j = 0; j < key; j++)
        {
            k = k * ct;
            k = k % n;
        }
        pt = k + 96;
        m[i] = pt;
        i++;
    }
    m[i] = -1;
    string dec;
    cout << "\nTHE DECRYPTED MESSAGE IS\n";
    for (i = 0; m[i] != -1; i++){
    char me = m[i];
        dec += me;
    }
    return dec;
}

int main()
{
    ifstream encryptedfile("encrypted_file.enc");
    string line,enc;
    if(encryptedfile.is_open()){
        while(getline(encryptedfile,line)){
            enc += line;
        }
    } else {
        cout << "eror in enc file\n";
    }
    string line1;
    vector <string> v;
    ifstream pub_key("key.key");
    if(pub_key.is_open()){
        while(getline(pub_key,line1)){
            v = explode(line1,' ');
        }
    } else{
        cout << "error in key file\n";
    }
    int n = stoi(v[0]); // prime number multiplication as we use in RSA
    int d = stoi(v[1]); // public key 
    string dec = decrypt(n, d, enc);

    BTreeNode *root = NULL;
    BTreeNode *head ;
    ifstream myfile("output.txt");
    if(myfile.is_open()){
        while(getline(myfile,line)){
            auto v = explode(line,' ');
            root = insert(root, v);
            v.clear();
        }
    }
    
    
    v = explode(dec,'S');
    for(int i = 0; i < v.size(); i++){
        cout << v[i] << " ";
    }
    cout << endl;
    cout <<"data recieved : " <<endl;
    traverse(root);
int s1 = stoi(v[0]) - 1;
int s2 = stoi(v[v.size()-1]);
    BTreeNode *st = search(root,s1);
    BTreeNode *en = search(root,s2);
    string hashvalue = hashcompute(root) ;
    string rt = hashrange(root,st,en,s1,s2);
    
    cout << "hashvalue of data " << hashvalue <<endl;
    ifstream h1 ("hash.hsh");

    /*if(h1.is_open()){
        getline(h1,line1);
        cout << line1;
    }*/
        line1 = hashvalue;
    int i;
    for(i = 0; i < 64; i++){
        if(hashvalue[i] != line1[i]){
            cout << "Data is not correct\n";
            break;
        }
    }
    if(i == 64) cout << "Data receivd is correct\n";
    /*BTreeNode *ptr = st;
    string msg;
    while(ptr){
        for(int i = 0; i < ptr-> n; i++){
            
            if(ptr->keys[i] >= s1 && ptr->keys[i] <= s2){
                stringstream ss;
                ss << ptr->keys[i];
                op << ss.str() << " " << ptr->data[i] << endl;
                string newstring = ss.str();
                msg.append(newstring);
            }
        }
        ptr = ptr->next;
    }
     op.close();
     if( msg[0]== '\0')
        cout << "Nothing found\n";
    else {
        rsa(msg);
        pub_key << n << " " << d[0] << endl;
        encryptedfile << encrypt(msg) << endl;
    }
    if(!searchvalue(root,s1,s2)){
        cout <<"could not find " << endl;
    }*/
    return 0;
}
