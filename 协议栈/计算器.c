#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>
#define maxsize 1000

typedef struct{
     double data[1000];
     int t;
}dStack;
void dstk_create(dStack *dstk){
    dstk->t=0;
    dstk->data[0]=0;
}
void dstk_clear(dStack *dstk){
    dstk->t=0;
}
double dstk_pop(dStack *dstk){
    if (dstk->t<=0) {
        printf("dstack is empty\n");
        return dstk->data[0];
    }
    dstk->t--;
    return dstk->data[dstk->t];
}
void dstk_push(dStack *dstk ,double x){
    if(dstk->t>=1000){
        printf("the dstack is full\n");
        return;
    }
    dstk->data[dstk->t++]=x;
}
double dstk_top(dStack *dstk){
    if(dstk->t<=0){
        printf("the dstack is empty\n");
        return dstk->data[0];
    }
        return dstk->data[dstk->t-1];
    }
int dstk_is_empty(dStack *dstk){
    if(dstk->t<=0) return 1;
    return 0;
}


typedef struct{
     char data[1000];
     int t;
}sStack;
void sstk_create(sStack *sstk){
    sstk->t=0;
    sstk->data[0]=0;
}
void sstk_clear(sStack *sstk){
    sstk->t=0;
}
char sstk_pop(sStack *sstk){
    if (sstk->t<=0) {
        printf("sstack is empty\n");
        return sstk->data[0];
    }
    sstk->t--;
    return sstk->data[sstk->t];
}
void sstk_push(sStack *sstk ,char x){
    if(sstk->t>=1000){
        printf("the sstack is full\n");
        return;
    }
    sstk->data[sstk->t++]=x;
}
char sstk_top(sStack *sstk){
    if(sstk->t<=0){
        printf("the sstack is empty\n");
        return sstk->data[0];
    }
        return sstk->data[sstk->t-1];
    }
int sstk_is_empty(sStack *sstk){
    if(sstk->t<=0) return 1;
    return 0;
}





typedef struct{
     char pExp[200];
     char iExp[100];
     double n;
}Cal;
double Cal_cal_p(Cal *cal);
void Cal_create(Cal *cal,char *s){
    printf("a1\n");
    strcpy(cal->iExp,s);
    printf("a2\n");
    cal->n=0;
    printf("a3\n");
}
void Cal_disp(Cal *cal)
{
    printf("后缀表达式为：");
    printf("%s\n",cal->pExp);
    printf("表达式的值为：");
    printf("%.2f\n",cal->n);
}
double Cal_cal(Cal *cal,char *iE){
    strcpy(cal->iExp, iE);
    if (Cal_i_to_p(cal)){
        return cal->n=Cal_cal_p(cal);
    }else{
        printf("语法错误，请检查表达式\n");
        return 0.0;
    }
}
double read_num( char* s,int *pos)
{
    double x=0;
    int flag=0,w=0,h=1,i;
    if(s[(*pos)]=='-') {(*pos)++;h=-1;}
    for ( i=(*pos);i<strlen(s);i++)
    if (s[i]!='#')
        if(s[i]=='.'){
            flag=1;
        }else {
            if(flag) w++;
            x=x*10+s[i]-'0';
    }else  break;
    (*pos)=i;
    return x/pow(10,w)*h;
}
char *change(double x){
     char ss[20];
     gcvt(x,4,ss);
     return  ss;
}
double calL(double num1,double num2,char op)
{
    switch(op){
  case '+':
      return num1+num2;
  case '-':
      return num1-num2;
  case '*':
      return num1*num2;
  case '/':
      if(num2==0)
      {printf("除数不能为0\n"); break;}
      else
        return num1/num2;
  default:
     return 0.0;
    }
}

int get_pri(char c){
    switch(c){
      case '+':case '-': return 1;
      case '*':case '/': return 2;
      case '(':return 0;
      case ')':return 0;
      case '#':return -2;
      default:
        return -1;
    }
}
int Cal_i_to_p(Cal *cal)
{
    sStack stk;  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    sstk_create(&stk);
    int a_num_end=0,is_negative=0;
    int i;
    char result[200];
    //printf("%s\n",result);
    strcat(result,"#");
    //printf("%s\n",result);
    //printf("%c\n",result[1]);
    sstk_push(&stk,'#');
    for (i=0;i<strlen(cal->iExp);i++)
    {
        //printf("a1\n");
        char c = cal->iExp[i];
        //printf("a2\n");
        if(c=='-'&&(i==0||cal->iExp[i-1]=='(')) {strcat(result,"-"); is_negative=1;continue;}
        //printf("a2\n");
        if((c<='9'&&c>='0')||c=='.') {
                if(a_num_end&&!is_negative) {strcat(result,"#"); }
                //printf("a2\n");
                is_negative=0;
                char cc[10];
                cc[0]=c;
                //printf("%c\n",c);
                //printf("%s\n",result);
                //printf("%d\n",strlen(result));
                strncat(result,cc,1);
                //printf("%s\n",result);
                //printf("a0\n");
                a_num_end=0;
        }
        else{a_num_end=1;
            //printf("a3\n");
            if(c=='(') sstk_push(&stk,'(');
            else if (c==')') {
                while (sstk_top(&stk)!='(')
                {
                    strcat(result,"#");
                    char cc = sstk_pop(&stk);
                    strncat(result,&cc,1);
                }
                sstk_pop(&stk);
            }

            else {
             int p = get_pri(c);
             if (p==-1) return 0;
             //printf("%c\n",sstk_top(&stk));
             //printf("%s\n",result);
             while (p<=get_pri(sstk_top(&stk))){
                strcat(result,"#");
                char cc = sstk_pop(&stk);
                strncat(result,&cc,1);
                //printf("%s\n",result);
             }
             //printf("a4\n");
             sstk_push(&stk,c);
             //printf("a5\n");
        }
        }
        //printf("%d\n",i);
    }
    //printf("a7\n");
    while (sstk_top(&stk)!='#'){
        strcat(result,"#");
        char cc = sstk_pop(&stk);
        strncat(result,&cc,1);
        //printf("a6\n");
    }
    strcat(result,"#");
    //printf("%s\n",result);
    strcpy(cal->pExp , result);
    return 1;
}
double Cal_cal_p(Cal *cal)
{
    int l = strlen(cal->pExp);
    dStack stk1;//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    dstk_create(&stk1);
    int i;
    for ( i=1;i<l;i++){
        char c = cal->pExp[i];
        if (c=='#') continue;
        if((c=='-'&&cal->pExp[i+1]!='#')||(c>='0'&&c<='9')) dstk_push(&stk1,read_num(cal->pExp,&i));
        else {
           double y  = dstk_pop(&stk1);
           double x = dstk_pop(&stk1);
           dstk_push(&stk1,calL(x,y,c));
        }
    }
   return dstk_pop(&stk1);

}
int main()
{
    Cal c;
    char ini[100];
    while (1){
        printf("输入要计算的表达式(以字母Q结束程序)：\n");
        scanf("%s" ,&ini);
        if(strlen(ini)==1&&ini[0]=='Q') break;
        Cal_cal(&c,ini);
        //Cal_create(&c,ini);
        Cal_disp(&c);
    }

    return 0;
}
