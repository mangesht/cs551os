
struct assoc_ar{
    int max_cur_size;
    int max_ov_tbl_size;
    int cur_size;
    int ov_tbl_size;
    char **key_tbl;
    char **val_tbl;
    int *hit_idx;
};

void init(struct assoc_ar* ar){
    int i;
    ar->max_cur_size = 256;
    ar->max_ov_tbl_size = 8;
    ar->cur_size = 0;
    ar->ov_tbl_size = 0 ;
    ar->key_tbl = (char **)malloc((ar->max_cur_size+ar->max_ov_tbl_size) * sizeof(char *));
    ar->val_tbl = (char **)malloc((ar->max_cur_size+ar->max_ov_tbl_size) * sizeof(char *));
    ar->hit_idx = (int *) malloc(ar->max_cur_size * sizeof(int));
    for(i=0;i< (ar->max_cur_size+ar->max_ov_tbl_size); i++) {
        ar->key_tbl[i] = (char *) malloc(256);
        ar->val_tbl[i] = (char *) malloc(256);
        if(i<ar->max_cur_size) ar->hit_idx[i] = -1;
    }
}

unsigned char get_hash(char *p){
    int i;
    int sum = 0;
    for(i=0;p[i] != NULL; i++){
        sum = sum + (int) *(p+i);
    }
    return (unsigned char)(sum =(sum & 0xFF)+((sum >> 8) &0xFF)) & 0xFF;
}



int add(struct assoc_ar *ar,char *key,char *val){
    int hash;
    char *p;
    int i;
    hash = get_hash(key);
    if(debug_en) printf("hash = %d \n",hash); 
    if(ar->hit_idx[hash] == -1 ) {
        // The entry is empty. Creat New entry
        p = strcpy(ar->key_tbl[hash],key);
        p = strcpy(ar->val_tbl[hash],val);
        ar->cur_size++;
    }else{
        if(strcmp(key,ar->key_tbl[hash])==0) {
            // Match found in main table. Replace the entry
            p = strcpy(ar->val_tbl[hash],val);
        }else{
            // Search the overflow table
            for(i=ar->max_cur_size;i<(ar->max_cur_size+ar->ov_tbl_size);i++){
                if(strcmp(key,ar->key_tbl[i])==0){
                    break;
                }
            }
            if(i == (ar->max_cur_size+ar->ov_tbl_size)){
                // Match not found, this is new entry
                p = strcpy(ar->key_tbl[i],key);
                p = strcpy(ar->val_tbl[i],val);
                ar->ov_tbl_size++;
                if(ar->ov_tbl_size >= ar->max_ov_tbl_size){
                    printf("Hash Table full. Contact your provider for resizing request\n");
                } 
            }else{
                // Entry is already present , replace
                p = strcpy(ar->val_tbl[i],val);
            }
        }
    }
    ar->hit_idx[hash]++;
}

char * get(struct assoc_ar *ar,char *key){
    int hash;
    char *p;
    char *ret_val;
    int i;
    hash = get_hash(key);
    if(ar->hit_idx[hash] == -1 ) {
        // The entry is empty. Creat New entry
        return NULL;
    }else{
        if(strcmp(key,ar->key_tbl[hash])==0) {
            // Match found in main table. Replace the entry
            ret_val = (char *) malloc(256);
            p = strcpy(ret_val,ar->val_tbl[hash]);
            if(debug_en) printf("get success returning %s\n",ret_val);
            return ret_val;
        }else{
            // Search the overflow table
            for(i=ar->max_cur_size;i<(ar->max_cur_size+ar->ov_tbl_size);i++){
                if(strcmp(key,ar->key_tbl[i])==0){
                    break;
                }
            }
            if(i == (ar->max_cur_size+ar->ov_tbl_size)){
                // Match not found, this is new entry
                if(debug_en) printf("get failure\n");
                return NULL;
            }else{
                ret_val = (char *) malloc(256);
                p = strcpy(ret_val,ar->val_tbl[i]);
                if(debug_en) printf("get success\n");
                return ret_val;
            }
        }
    }
    return ret_val;
}
void write_alias(struct assoc_ar *ar,int fd){
    char *p;
    int i;
    if(debug_en) printf("Processing write alias\n");
    for(i=0;i<ar->max_cur_size;i++){
        if(ar->hit_idx[i] == -1) {
            // Empty Location
        }else{
            // alias=6+spalce 7 , null 8  newline 9  = 10 
            if(debug_en) printf("Processing %dth write alias\n",i);
            p = (char *) malloc((10+strlen(ar->key_tbl[i])+strlen(ar->val_tbl[i])) * sizeof(char));
            strcpy(p,"alias ");
            strcat(p,ar->key_tbl[i]);
            strcat(p,"=");
            strcat(p,ar->val_tbl[i]);
            strcat(p,"\n");
            write(fd,p,strlen(p));
            if(debug_en) printf("%s",p);
        }
    }
    //    close(fd);
}

void print_array(struct assoc_ar* ar){
    int i=0;
    for(i=0;i<8;i++){
        printf("value = %s\n",ar->key_tbl[i]);
    }
    
}

/*
char * read_command(){
    char ch; 
    int len = 0;
    int ret_val;
    char * cmd = (char *) malloc(256);
    ch = 0 ; 
    while(ch!=10){
        ret_val = scanf("%c",&ch); 
        if(ret_val == -1) {
            perror("scan error");
            break;
        }
        *(cmd+len) = ch;
        len++;
    }
    *(cmd+len-1) = 0;
    //printf("scanDone %s",cmd);
    //printf("cmd = %x str = %x \n",cmd,str);
    return cmd;
}


int main(){
    char *p = "Home";
    char *key;
    char *val;
    struct assoc_ar alias_s;
    
    init(&alias_s);
    p = (char *) malloc(256);
    key = (char *) malloc(256);
    val = (char *) malloc(256);
    while(1) {
        printf("Enter your Choice 1 : Set value 2 : Get value  ");
        p = read_command();  
        printf("Enter Key ");
        key = read_command();
        if(strcmp(p,"1")==0) {
            printf("Enter Value ");
            val = read_command();
            add(&alias_s,key,val);
            if((val=get(&alias_s,key)) != NULL ) printf("Added key =%s Val = %s \n",key,val);
            else printf("Error: Could not get for key %s \n",key);
        }else{
            if((val=get(&alias_s,key)) != NULL ) printf("Added key =%s Val = %s \n",key,val);
            else printf("Error: Could not get for key %s \n",key);
        }
        //printf("hash for %s = %d\n",p,(int)get_hash(p));
    }
    return 0;
    
}

*/
