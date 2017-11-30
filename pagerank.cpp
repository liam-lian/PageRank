#include <stdio.h>
#include <stdlib.h>

#include<math.h> 
#include<string.h>
#include<map>

using namespace std; 
#define THRESHOLD 0.15

/*
	使用了改进的ELL方式进行矩阵的存储。
	压缩矩阵本身的行就是原来数据的行。
	每一行使用一个链表连接起来，将属于改行的全部数据放在链表中。
	链表节点存储数据内容以及在本行中的列
*/

//稀疏矩阵中的元素
//使用链表链接起来
typedef struct MatrixNode{
	int row;
	double val;
	struct MatrixNode* next;
}MatrixNode;

typedef struct directpair{
	int i,j;
}directpair;

typedef struct Respair
{
	double val;
	int index;
}Respair;

directpair Pagepair;
int getDirect(char *Line){
	char *line=Line;
	int i=0,j=0;
	while(*line>='0'&&*line<='9'){
		i=i*10+(*line++-'0');
	}
	while(line){
		
		if(*line>='0'&&*line<='9'){
			j=*line++-'0';
			while(*line>='0'&&*line<='9'){
				j=j*10+(*line++-'0');
			}
			break;
		}
		line++;
	}
	Pagepair.i=i;
	Pagepair.j=j;
	return 1;
}
map<int,char*> urlMap;
void saveurl(char *Line){
	char * line=Line;
	int No=0;
	char * url=(char*)malloc(200);
	char * purl=url;
	while(*line!=' '){
		*purl++=*line++;
	}
	*purl='\0';
	while(*line++){
		if(*line>='0' && *line<='9')
			No=No*10+(*line-'0');
	}
	urlMap[No]=url;
}


char* geturl(int No){

	map<int,char*>::iterator it;
	it=urlMap.find(No);
    if(it!=urlMap.end()){
    	return it->second;
    }
    return NULL;
}


int compArr(double* arr1,double* arr2,int len){
	

	int i=0;
	for (int i = 0; i < len; ++i)
	{
		if(fabs(arr1[i]-arr2[i])>THRESHOLD)
			return 1;
	}
	return 0;
}


int cmp(const void * a, const void * b)
{
     return ((Respair*)b)->val-((Respair*)a)->val>0?1:-1;
}

//pagerank url.txt top10.txt
int main(int argc, char *argv[]){

    if(argc<3){
    	printf("the arg nuumber is too less\n");
    }
 	FILE* fp=fopen(argv[1],"r");
 	FILE* fpOut=fopen(argv[2],"w");

 	if(fp==NULL || fpOut==NULL){
 		printf("can not find some file\n");
 		return -1;
 	}

	int i,j;
	//矩阵的初始维度，设置一个足够大的值，确保矩阵不会越界
	int MaxtrixSize=50*10000;
	//矩阵的实际维度,在读入数据的过程中，找到最大的编号值，作为矩阵的实际维度
	int MatrixRealSize=-1;

	//阻尼系数	
	double alpha=0.015;
    //稀疏矩阵定义
	MatrixNode* matrix[MaxtrixSize];
	MatrixNode* tailVector[MaxtrixSize];
	

	//初始化稀疏矩阵
	for (int i = 0; i < MaxtrixSize; ++i){
		matrix[i]=(MatrixNode*)malloc(sizeof(MatrixNode));
		tailVector[i]=matrix[i];
	}

	//记录i出发指向的所有的页面编号
	// int from_prei_to[MaxtrixSize];

	int *from_prei_to=(int *)malloc(sizeof(int)*MaxtrixSize);


	//from_prei_to 的长度
	int index=0;

	directpair prePair;
	
	char strLine[200];

	while(1){
		fgets(strLine,200,fp);
		if(strLine[0]=='\n')
			break;
		saveurl(strLine);
	}
	

    //读取矩阵第一行
	if(!feof(fp)){ 
		fgets(strLine,100,fp);
		getDirect(strLine);
		prePair.i=Pagepair.i;
		from_prei_to[index++]=Pagepair.j;
		if(MatrixRealSize<Pagepair.j){
			MatrixRealSize=Pagepair.j;
		}
	}

	int ii=0;
	//循环计算
	while(1){
		
		memset(strLine,0,100);

		fgets(strLine,100,fp);
		getDirect(strLine);

		if(MatrixRealSize<Pagepair.j){
			MatrixRealSize=Pagepair.j;
		}

		if(prePair.i==Pagepair.i){
			from_prei_to[index++]=Pagepair.j;
		}
		else{
			for (i = 0; i < index; ++i){
				MatrixNode *newNode=(MatrixNode*)malloc(sizeof(MatrixNode));
				newNode->row=prePair.i;
				
			    newNode->val=1.0/index * (1.0-alpha); //增加修正的权值
				newNode->next=NULL;
				tailVector[from_prei_to[i]]->next=newNode;
				tailVector[from_prei_to[i]]=newNode;
			}
			index=0;
			prePair.i=Pagepair.i;
			from_prei_to[index++]=Pagepair.j;
		}

		//solve the last line read two times
		if(feof(fp)){
			if(MatrixRealSize<Pagepair.i){
				MatrixRealSize=Pagepair.i;
			}
			break;
		}
	}

	MatrixRealSize++;

    //二维数组指代的是ri和ri+1,两者所代表的内容，交替的进行这变化，这样做可以避免内存拷贝的损耗。
	//使用一个flag标志ri

	double* Array_i_=(double*)malloc(MatrixRealSize*2*sizeof(double));
	int Array_i_flag=0;

	//初始值，计算向量的入度以及总的边的个数
	int edgecount=0;
	for (i = 0; i < MatrixRealSize; ++i){
		MatrixNode *node=matrix[i]->next;
		while(node){
			Array_i_[Array_i_flag*MatrixRealSize+i]++;
			node=node->next;
		}
		edgecount+=Array_i_[Array_i_flag*MatrixRealSize+i];
	}
	for (i = 0; i < MatrixRealSize; ++i){
		Array_i_[Array_i_flag*MatrixRealSize+i]/=edgecount;
	}

	int running=1;

	while(running){

		int Array_i_Sum=0;
		//sum of array
		for (i = 0; i <MatrixRealSize; ++i){
			Array_i_Sum+=Array_i_[Array_i_flag*MatrixRealSize+i];
		}

		//calculate v(i+1)
		int index_I_1=(Array_i_flag+1)%2;
		memset(Array_i_+index_I_1*MatrixRealSize,0,MatrixRealSize*sizeof(double));

		for (i = 0; i < MatrixRealSize; ++i){
			MatrixNode *node=matrix[i]->next;

			while(node){
				Array_i_[index_I_1*MatrixRealSize+i] += node->val* Array_i_[Array_i_flag*MatrixRealSize+node->row];
				node=node->next;
			}
			//调整修正
			Array_i_[index_I_1*MatrixRealSize+i]+=alpha*Array_i_Sum/MatrixRealSize;	
		}

	    running=compArr(Array_i_,Array_i_+MatrixRealSize,MatrixRealSize);
		Array_i_flag=(Array_i_flag+1)%2;
	}

	double* theres=Array_i_+((Array_i_flag+1)%2)*MatrixRealSize;
	
	Respair* calres=(Respair*)malloc(sizeof(Respair)*MatrixRealSize);
	for(i=0;i<MatrixRealSize;i++){
		calres[i].val=theres[i];
		calres[i].index=i;
	}
	qsort(calres,MatrixRealSize,sizeof(Respair),cmp);


	char *url;
	int count=0;
	i=0;
	while(count<10){
		if(url=geturl(calres[i].index)){
			fprintf(fpOut,"%s %lf\n",url,calres[i].val);
			count++;
		}	
		i++;
	}
}
