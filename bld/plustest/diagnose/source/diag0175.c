??=error trigraph ??= test ??/
error message
char *foo( char *buff )
{
    return new (buff) char[10];
}

// be careful the following line is \<space><space><TAB><TAB><space><space>
int aa\  		  
bb;

int aa??/  ??(23??);

void foo() {
    aabb = 1;
}
