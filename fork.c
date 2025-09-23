int main(){
    if(fork()){
        printf("non-zero\n");
    } else {
        printf("zero\n");
    }
}