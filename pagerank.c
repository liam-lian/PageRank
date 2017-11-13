#include <stdio.h>
#include <stdlib.h>

#define THRESHOLD 1

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

directpair pair;
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
	pair.i=i;
	pair.j=j;
	// free(Line);
	return 1;
}

void printpair(directpair pair){
	printf("%d %d\n",pair.i,pair.j);
}
void printArr(double*cc,int len ){
	for (int i = 0; i < len; ++i)
	{
		printf("%lf ",cc[i]);
	}
	printf("\n");
}


//二维数组可以转化为一维数组传入。因为数组是连续存储的
int compArr(double* arr,int len){
	int i=0;
	for (i = 0; i < len; ++i){
		if(arr[i]-arr[len+i]>THRESHOLD || arr[len+i]-arr[i]>THRESHOLD){
			return 1;
		}
	}
	return 0;
}

int main(){

	int i,j;
	//矩阵的维度
	int MaxtrixSize=7;
	//阻尼系数	
	double alpha=0.15;
    //稀疏矩阵定义
	MatrixNode* matrix[MaxtrixSize];
	MatrixNode* tailVector[MaxtrixSize];

	//初始化稀疏矩阵
	for (int i = 0; i < MaxtrixSize; ++i){
		matrix[i]=(MatrixNode*)malloc(sizeof(MatrixNode));
		tailVector[i]=matrix[i];
	}

	//记录i出发指向的所有的页面编号
	int from_prei_to[MaxtrixSize];
	//from_prei_to 的长度
	int index=0;

	directpair prePair;
	FILE* fp=fopen("testdata","r");
	char strLine[100];
    //读取矩阵第一行
	if(!feof(fp)){ 
		fgets(strLine,100,fp);
		getDirect(strLine);
		prePair.i=pair.i;
		from_prei_to[index++]=pair.j;
	}

	//循环计算
	while(!feof(fp)){

		fgets(strLine,100,fp);
		getDirect(strLine);

		printpair(pair);

		if(prePair.i==pair.i){
			from_prei_to[index++]=pair.j;
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
			prePair.i=pair.i;
			from_prei_to[index++]=pair.j;
		}
	}

	//打印结果，至此矩阵构造完毕
	for (int i = 0; i < MaxtrixSize; ++i){
		MatrixNode *node=matrix[i]->next;
		while(node){
			printf("[%d--%d--%lf]\n",i,node->row,node->val);
			node=node->next;
		}
	}

    //二维数组指代的是ri和ri+1,两者所代表的内容，交替的进行这变化，这样做可以避免内存拷贝的损耗。
	//使用一个flag标志ri
	double Array_i_[2][MaxtrixSize];
	int Array_i_flag=0;

	//初始值，计算向量的入度以及总的边的个数
	int edgecount=0;
	for (i = 0; i < MaxtrixSize; ++i){
		MatrixNode *node=matrix[i]->next;
		while(node){
			Array_i_[0][i]++;
			node=node->next;
		}
		edgecount+=Array_i_[0][i];
	}
	for (i = 0; i < MaxtrixSize; ++i){
		Array_i_[0][i]/=edgecount;
		printf("%lf ", Array_i_[0][i]);
	}


	int running=1;

	while(running){


		int Array_i_Sum=0;
		for (i = 0; i <MaxtrixSize; ++i){
			Array_i_Sum+=Array_i_[Array_i_flag];
		}

		//计算了一次矩阵乘法
		for (i = 0; i < MaxtrixSize; ++i){
			MatrixNode *node=matrix[i]->next;
			int index_I_1=(Array_i_flag+1)%2;

			while(node){
				printf("d\n");
				Array_i_[index_I_1][i] += node->val* Array_i_[Array_i_flag][node->row];
				node=node->next;
			}
			printf("dd\n");
			//调整修正
			Array_i_[index_I_1][i]+=alpha*Array_i_Sum;	
		}

		printf("oooooooooooo\n");
		running=compArr((double*)Array_i_,MaxtrixSize);
		printf("asdasd\n");
		Array_i_flag=(Array_i_flag+1)%2;
		printArr(Array_i_[Array_i_flag],MaxtrixSize);
	}

	for (int i = 0; i < MaxtrixSize; ++i)
	{
		printf("%s ",Array_i_[(Array_i_flag+1)%2][i]);
	}
}