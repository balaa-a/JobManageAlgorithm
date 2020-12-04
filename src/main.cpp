#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<memory.h>
#include<algorithm>
#include<vector>
#include<queue>
#include<fstream>
#include<iostream>
#define inf 0x3f3f3f
using namespace std;
const int MAX_NUM = 10; //最大作业数量
const int MAX_LINE = 1024; //一行的size
#define queuesize 10
#define MAXTIME 150 //考虑前100ms时间
#define MINLLF 9999
#define PRO_LRU_T 0



//定义作业信息结构
typedef struct node {
	int number;         //作业号
	int reach_time;     //作业抵达时间
	int need_time;      //作业的执行时间
	int privilege;	    //作业优先权
	int count_time;		//记录作业完成一段时间时候还有多少时间

	float excellent;    //响应比
	int start_time;     //作业开始时间
	int wait_time;      //等待时间
	int tr_time;        //周转时间
	double wtr_time;    //带权周转时间
	int run_time;       //作业累计已执行时间
	bool visited;		//作业是否被访问过
	int finish_time; //作业完成时间
	bool reached; //作业是否抵达
} job;
typedef struct PCB {
	int id;//任务序号
	int arrivetime;//任务到达时间
	int runtime;//任务需要执行的时间
	int counttime;//记录任务处理一段时间后,剩下的需要处理的时间
	struct PCB* next;
}*task, pcb;


int num = 0; //实际作业数量
job jobs[MAX_NUM]; //作业
int ready[MAX_NUM];//就绪队列
int order[MAX_NUM];//记录排序使用哪个数值作为排序对象



pcb* creattask(int x, int y, int z) {
	task newtask = (task)malloc(sizeof(pcb));
	newtask->id = x;
	newtask->arrivetime = y;
	newtask->runtime = z;
	newtask->counttime = z;
	newtask->next = NULL;
	return newtask;
}
void deltask(pcb* n, int x) {
	task d;
	if (n != NULL) {
		while (n->next->id != x) {
			n = n->next;
		}
		d = n->next;
		n->next = d->next;
		free(d);
	}
}
void count(pcb* n, int t) {
	pcb* q, * p;//q用于记录最先到达的任务位置，p用于遍历链表
	int qp=0; //用于记录上一次任务的id号
	int temp = t; float time = 0; //time记录当前时间，temp记录任务个数,便于后续操作
	float zt = 0, dt = 0;  //zt用于记录总周转时间,dt记录总带权周转时间
	fstream output("data/out/6.txt");
	output<<"作业标号\t周转时间\t带权周转时间"<<endl;
	while (t != 0) {
		p = n->next;
		q = p;
			while (p != NULL) { //找到最先到达的任务
				if (p->arrivetime < q->arrivetime) {
					q = p;
				}
				p = p->next;
			}
			p = n->next;
			while (p != NULL) { //找到处于就绪的多个任务里面任务最短的任务
				if ((p->arrivetime <= time || p->arrivetime == q->arrivetime) && p->counttime < q->counttime)
					q = p;
				p = p->next;
			}
			q->counttime--;
			if(qp==0||qp!=q->id)
//			cout<<"当前执行的任务序号为:"<< q->id<<endl;
			qp = q->id;
			if (time < q->arrivetime)//防止时间暂停
				time = q->arrivetime;
			time++;
		if (q->counttime== 0) {
			output<<q->id<<"\t\t"<<q->arrivetime<<"\t\t";
			zt = zt + time - q->arrivetime;
			output<<(time - q->arrivetime) / q->runtime<<endl;
			dt = dt + (time - q->arrivetime) / q->runtime;
			deltask(n, q->id);
			--t;
		}
	}

	output<<"--------所有作业调度完毕------\n";
	output<<"平均周转时间: "<<zt/temp<<"\n平均带权周转时间: "<<dt/temp<<endl;
}

void CRT() {
	int num;
	task tail = NULL;
	task head = NULL;
	tail = (task)malloc(sizeof(pcb));
	head = tail;

	fstream input("data/in/data.txt");
	if (input.fail()) {
		cout << "打开文件失败\n";
		exit(0);
	}

	int a, b, c, d;
	num=1;

	while (1) {
		if(!input.eof()){
			input>> a >> b >> c >> d;
			tail->next = creattask(a, b, c);
			tail = tail->next;
			num++;
			continue;
		}else
			break;
	}
	input.close();
	num=num-1;

	count(head, num);
}

void initial_jobs() { //初始化所有作业信息
	for (int i = 0; i < MAX_NUM; i++) {
		jobs[i].excellent = 0;
		jobs[i].need_time = 0;
		jobs[i].number = 0;
		jobs[i].privilege = 0;
		jobs[i].reach_time = 0;
		jobs[i].run_time = 0;
		jobs[i].start_time = 0;
		jobs[i].tr_time = 0;
		jobs[i].visited = false;
		jobs[i].wait_time = 0;
		jobs[i].wtr_time = 0;
		jobs[i].finish_time = 0;
		jobs[i].reached = false;
	}
}

void read_Jobdata() {
	fstream input("data/in/data.txt");
	if (input.fail()) {
		cout << "打开文件失败\n";
		exit(0);
	}

	int a, b, c, d;
	num=1;

	while (1) {
		if(!input.eof()){
			input>> a >> b >> c >> d;
			jobs[num].number= a;
			jobs[num].reach_time = b;
			jobs[num].need_time = c;
			jobs[num].privilege = d;
			num++;
			continue;
		}else
			break;
	}
	num--;
	cout << "作业号\t" << "到达时间\t" << "执行时间\t" << "优先权" << endl;
	for (int j = 1; j<=num; j++) {
		cout << jobs[j].number << "\t" << jobs[j].reach_time << "\t"
				<< jobs[j].need_time << "\t" << jobs[j].privilege << endl;
	}
	input.close();
}

void initial_jobs_again() {
	for (int i = 1; i <= num; i++) {
		jobs[i].finish_time = 0;
		jobs[i].excellent = 0;
		jobs[i].tr_time = 0;
		jobs[i].wtr_time = 0;
		jobs[i].wait_time = 0;
		jobs[i].visited = false;
	}
}

//调度函数

//先来先服务算法
void FCFS() {
	int i, j, temp;
	double sum1 = 0; //总的等待时间
	double sum2 = 0; //总的周转时间
	double sum3 = 0; //总的带权周转时间
	fstream output("data/out/1.txt");
	cout<<"FCFS作业调度算法"<<endl;

	for (i = 1; i <= num; i++) {
		order[i] = jobs[i].reach_time;
		ready[i] = i;
	}

	//冒泡排序
	for (i = 1; i <= num; i++) { //按照到达时间大小排序
		for (j = 1; j <= num - i; j++) {
			if (order[j] > order[j + 1]) {
				temp = order[j];
				order[j] = order[j + 1];
				order[j + 1] = temp;
				temp = ready[j];
				ready[j] = ready[j + 1];
				ready[j + 1] = temp;
			}
		}
	}

	output<<"进程标号\t到达时间\t服务时间\t等待时间\t周转时间\t带权周转时间"<<endl;
	for (i = 1; i <= num; i++) {
		output<<ready[i]<<"\t\t"<<jobs[ready[i]].reach_time<<"\t\t"<<jobs[ready[i]].need_time<<"\t\t";

		if (i == 1) {
			jobs[ready[i]].finish_time = jobs[ready[i]].reach_time
					+ jobs[ready[i]].need_time;
			jobs[ready[i]].wait_time = 0;
		} else {
			if (jobs[ready[i - 1]].finish_time > jobs[ready[i]].reach_time) { //如果上一个作业的完成时间大于下一个作业的到达时间，则下一个作业的开始时间从上一个的完成时间开始
				jobs[ready[i]].finish_time = jobs[ready[i - 1]].finish_time
						+ jobs[ready[i]].need_time;
				jobs[ready[i]].wait_time = jobs[ready[i - 1]].finish_time
						- jobs[ready[i]].reach_time;
			} else {
				jobs[ready[i]].finish_time = jobs[ready[i]].need_time
						+ jobs[ready[i]].reach_time;
				jobs[ready[i]].wait_time = 0;
			}
		}
		jobs[ready[i]].tr_time = jobs[ready[i]].finish_time
				- jobs[ready[i]].reach_time;
		jobs[ready[i]].wtr_time = double(jobs[ready[i]].tr_time)
				/ jobs[ready[i]].need_time;

		output<<jobs[ready[i]].wait_time<<"\t\t"<<jobs[ready[i]].tr_time<<"\t\t"<<jobs[ready[i]].wtr_time<<endl;
		sum1 += jobs[ready[i]].wait_time;
		sum2 += jobs[ready[i]].tr_time;
		sum3 += jobs[ready[i]].wtr_time;
	}
	output<<"--------所有作业调度完毕------\n";
	output<<"平均等待时间: "<<sum1/num<<"\n平均周转时间: "<<sum2/num<<"\n平均带群周转时间: "<<sum3/num;
	initial_jobs_again();

}


int findNextSJF(job j[MAX_NUM], int length, int time) {
	// p是已经到达且拥有最短运行时间的进程的下标
	// q是没有到达的进程中拥有最早到达时间的进程的下标
	int i, p, q;
	int minNeedTime, minReachTime, minTime;
	p = q = -1;
	minTime = minNeedTime = minReachTime = inf;
	for (i = 1; i <= length; i++) {
		if (!j[i].visited) {
			// 第一情况
			if (j[i].reach_time <= time && j[i].need_time <= minNeedTime) {
				p = i;
				minNeedTime = j[i].need_time;
			}
			// 第二种情况
			else if (j[i].reach_time > time
					&& j[i].reach_time <= minReachTime) {
				if (j[i].need_time < minTime) {
					q = i;
					minReachTime = j[i].reach_time;
					minTime = j[i].need_time;
				}
			}
		}
	}
	// p为-1时,代表在lastTime时刻还没进程到达,此时选择下一个最早到达的进程q
	if (p != -1)
		return p;
	return q;
}

//短作业优先算法
void SJF() {
	int i, j;
	double sum1 = 0; //总的等待时间
	double sum2 = 0; //总的周转时间
	double sum3 = 0; //总的带权周转时间
	int finish = inf; //当前完成时间
	fstream output("data/out/2.txt");
	cout<<"SJF作业调度算法"<<endl;
	for (i = 1; i <= num; i++) {
		finish = min(finish, jobs[i].reach_time);
	}
	output<<"进程标号\t到达时间\t服务时间\t等待时间\t周转时间\t带权周转时间"<<endl;

	for (i = 1; i <= num; i++) {
		int index = findNextSJF(jobs, num, finish);
		output<<index<<"\t\t"<<jobs[index].reach_time<<"\t\t"<<jobs[index].need_time<<"\t\t";

		if (jobs[index].reach_time <= finish) {
			jobs[index].wait_time = finish - jobs[index].reach_time;
			jobs[index].start_time = finish;
		} else {
			jobs[index].start_time = jobs[index].reach_time;
			jobs[index].wait_time = 0;
		}
		jobs[index].finish_time = jobs[index].start_time
				+ jobs[index].need_time;
		jobs[index].tr_time = jobs[index].finish_time - jobs[index].reach_time;
		jobs[index].wtr_time = (double) jobs[index].tr_time
				/ jobs[index].need_time;
		jobs[index].visited = true;
		sum1 += jobs[index].wait_time;
		sum2 += jobs[index].tr_time;
		sum3 += jobs[index].wtr_time;
		finish = jobs[index].finish_time;
		output<<jobs[index].wait_time<<"\t\t"<<jobs[index].tr_time<<"\t\t"<<jobs[index].wtr_time<<endl;
	}
	output<<"--------所有作业调度完毕------\n";
	output<<"平均等待时间: "<<sum1/num<<"\n平均周转时间: "<<sum2/num<<"\n平均带群周转时间: "<<sum3/num;
	initial_jobs_again();
}
int findNextHRRF(int pre) {
	int current = 1, i, j;    //优先权=(等待时间+要求服务时间)/要求服务时间
	for (i = 1; i <= num; i++) {
		if (!jobs[i].visited) {
			jobs[i].wait_time = jobs[pre].finish_time - jobs[i].reach_time; //等待时间=上一个进程的完成时间-到达时间
			jobs[i].excellent = (float) (jobs[i].wait_time + jobs[i].need_time)
					/ jobs[i].need_time;
		}
	}
	for (i = 1; i <= num; i++) {
		if (!jobs[i].visited) {
			current = i;    //找到第一个还没完成的作业
			break;
		}
	}
	for (j = i; j <= num; j++)    //和后面的作业比较
			{
		if (!jobs[j].visited)    //还没完成（运行）
		{
			if (jobs[current].reach_time < jobs[pre].finish_time) //如果进程在上一个进程完成之前到达
					{
				if (jobs[j].excellent > jobs[current].excellent)
					current = j;    //找出到达时间在上一个进程完成之前，优先权高的进程
			} else    //如果进程是在上一个进程完成之后到达
			{
				if (jobs[j].reach_time < jobs[current].reach_time)
					current = j;    //找出比较早到达的一个
				else if (jobs[j].reach_time == jobs[current].reach_time
						&& jobs[j].excellent > jobs[current].excellent) //如果同时到达
					current = j;    //找出服务时间比较短的一个
			}
		}
	}
	return current;    //返回当前进程
}
//高相应比优先
void HRRF() {
	int i;
	double sum1 = 0; //总的等待时间
	double sum2 = 0; //总的周转时间
	double sum3 = 0; //总的带权周转时间
	fstream output("data/out/3.txt");
	cout<<"高响应比优先调度算法: \n";
	int pre = 1;
	jobs[1].finish_time = 0;
	output<<"进程标号\t到达时间\t服务时间\t等待时间\t周转时间\t带权周转时间"<<endl;
	for (i = 1; i <= num; i++) {
		int index = findNextHRRF(pre);
		output<<index<<"\t\t"<<jobs[index].reach_time<<"\t\t"<<jobs[index].need_time<<"\t\t";

		if (i == 1) {
			jobs[index].start_time = jobs[index].reach_time;
			jobs[index].finish_time = jobs[index].start_time
					+ jobs[index].need_time;
			jobs[index].tr_time = jobs[index].need_time;
			jobs[index].wtr_time = 1.00;
			jobs[index].wait_time = 0;
		} else {
			if (jobs[index].reach_time > jobs[pre].finish_time) {
				jobs[index].wait_time = 0;
				jobs[index].start_time = jobs[index].reach_time;
			} else {
				jobs[index].start_time = jobs[pre].finish_time;
				jobs[index].wait_time = jobs[pre].finish_time
						- jobs[index].reach_time;
			}
			jobs[index].finish_time = jobs[index].start_time
					+ jobs[index].need_time;
			jobs[index].tr_time = jobs[index].finish_time
					- jobs[index].reach_time;
			jobs[index].wtr_time = (double) jobs[index].tr_time
					/ jobs[index].need_time;
		}
		jobs[index].visited = true;
		pre = index;
		sum1 += jobs[index].wait_time;
		sum2 += jobs[index].tr_time;
		sum3 += jobs[index].wtr_time;
		output<<jobs[index].wait_time<<"\t\t"<<jobs[index].tr_time<<"\t\t"<<jobs[index].wtr_time<<endl;

	}
	output<<"--------所有作业调度完毕------\n";
	output<<"平均等待时间: "<<sum1/num<<"\n平均周转时间: "<<sum2/num<<"\n平均带群周转时间: "<<sum3/num;
	initial_jobs_again();
}
//按照先来先服务并使用时间片轮转
vector<job> jobList;     //jobList向量

int time_unit = 10;     //时间片长度
void initial_job_List() {    //给向量初始化
	for (int i = 1; i <= num; i++) {
		jobList.push_back(jobs[i]);
	}
}
bool cmp(job a, job b) {
	return a.reach_time < b.reach_time;
}
void RR() {
	double sum1 = 0; //总的等待时间
	double sum2 = 0; //总的周转时间
	double sum3 = 0; //总的带权周转时间
	fstream output("data/out/5.txt");
	cout<<"时间片轮转作业调度算法"<<endl;
	initial_job_List();
	queue<job> Ready;     //就绪队列
	sort(jobList.begin(), jobList.end(), cmp);
	int begin = (*jobList.begin()).reach_time;
	Ready.push(*jobList.begin());
	jobList.erase(jobList.begin());

	output<<"作业标号\t到达时间\t执行时间\t等待时间\t周转时间\t平均周转时间\n";
	while (!jobList.empty() || !Ready.empty()) {
		while (!jobList.empty() && begin >= (*jobList.begin()).reach_time) { //有新作业到达，加入就绪队列
			Ready.push(*jobList.begin());
			jobList.erase(jobList.begin());
		}
		if (Ready.front().finish_time + time_unit < Ready.front().need_time) { //时间片用完没运行完,加入队尾
//			printf("%d 号作业执行了 %d\n", Ready.front().number, time_unit);
			Ready.front().finish_time += time_unit;
			begin += time_unit;
			while (!jobList.empty() && begin >= (*jobList.begin()).reach_time) { //有新作业到达，加入就绪队列
				Ready.push(*jobList.begin());
				jobList.erase(jobList.begin());
			}

			Ready.push(Ready.front());
			Ready.pop();
		} else { //作业运行完
			if (Ready.front().need_time - Ready.front().finish_time
					< time_unit) {
				time_unit -= Ready.front().need_time
						- Ready.front().finish_time;
			} else {
				time_unit = 10;
			}
//			printf("%d 号作业执行了 %d\n", Ready.front().number, time_unit);
			begin += time_unit;
			Ready.front().finish_time = begin;
			Ready.front().wait_time = Ready.front().finish_time
					- Ready.front().reach_time - Ready.front().need_time;
			Ready.front().tr_time = Ready.front().finish_time
					- Ready.front().reach_time;
			Ready.front().wtr_time = (double) Ready.front().tr_time
					/ Ready.front().need_time;
			sum1 += Ready.front().wait_time;
			sum2 += Ready.front().tr_time;
			sum3 += Ready.front().wtr_time;
			output<<Ready.front().number<<"\t\t"<<Ready.front().reach_time<<"\t\t"<<Ready.front().need_time<<"\t\t"<<Ready.front().wait_time<<"\t\t"<<Ready.front().tr_time<<"\t\t"<<Ready.front().wtr_time<<endl;

			//从就绪队列中移除作业
			Ready.pop();

			if (Ready.empty() && !jobList.empty()) {
				sort(jobList.begin(), jobList.end(), cmp);
//				printf("找到当前最早的作业是: %d\n", (*jobList.begin()).number);
				begin = (*jobList.begin()).reach_time;

			}
			if (time_unit < 10) {
				time_unit = 10;
			}
		}
	}
	output<<"--------所有作业调度完毕------\n";
	output<<"平均等待时间: "<<sum1/num<<"\n平均周转时间: "<<sum2/num<<"\n平均带群周转时间: "<<sum3/num;
	initial_jobs_again();
}
int findNextHPF(job j[MAX_NUM], int length, int time) {
	//优先值越低 表示优先权越高
	// p是已经到达且拥有最高优先权的进程的下标
	// q是没有到达的进程中拥有最早到达时间的进程的下标
	int i, p, q;
	int minReachTime, minPrivilege1;
	p = q = -1;
	minPrivilege1 = minReachTime = inf;
	for (i = 1; i <= length; i++) {
		if (!j[i].visited) {
			// 第一情况
			if (j[i].reach_time <= time && j[i].privilege <= minPrivilege1) {

				if (j[i].privilege == j[p].privilege) {   //如果优先权一致 则按最早抵达时间
					if (j[i].reach_time < j[p].reach_time) {
						p = i;
					}
				} else {
					p = i;
					minPrivilege1 = j[i].privilege;
				}
			}
			// 第二种情况
			else if (j[i].reach_time > time
					&& j[i].reach_time <= minReachTime) {
				q = i;
				minReachTime = j[i].reach_time;
			}
		}
	}
	// p为-1时,代表在time时刻还没进程到达,此时选择下一个最早到达的进程q
	if (p != -1)
		return p;
	return q;
}
//优先权高者优先
void PSA() {
	int i;
	double sum1 = 0; //总的等待时间
	double sum2 = 0; //总的周转时间
	double sum3 = 0; //总的带权周转时间
	int finish = inf; //当前完成时间
	fstream output("data/out/4.txt");
	cout<<"优先级调度-非抢占式: \n";
	for (i = 1; i <= num; i++) {
		finish = min(finish, jobs[i].reach_time);
	}

	output<<"作业号\t到达时间\t服务时间\t等待时间\t周转时间\t带权周转时间"<<endl;
	for (i = 1; i <= num; i++) {
		int index = findNextHPF(jobs, num, finish);
		output<<jobs[index].number<<"\t\t"<<jobs[index].reach_time<<"\t\t"<<jobs[index].need_time<<"\t\t";
		if (jobs[index].reach_time <= finish) {
			jobs[index].wait_time = finish - jobs[index].reach_time;
			jobs[index].start_time = finish;
		} else {
			jobs[index].start_time = jobs[index].reach_time;
			jobs[index].wait_time = 0;
		}
		jobs[index].finish_time = jobs[index].start_time
				+ jobs[index].need_time;
		jobs[index].tr_time = jobs[index].finish_time - jobs[index].reach_time;
		jobs[index].wtr_time = (double) jobs[index].tr_time
				/ jobs[index].need_time;
		jobs[index].visited = true;
		sum1 += jobs[index].wait_time;
		sum2 += jobs[index].tr_time;
		sum3 += jobs[index].wtr_time;
		finish = jobs[index].finish_time;
		output<<jobs[index].wait_time<<"\t\t"<<jobs[index].tr_time<<"\t\t"<<jobs[index].wtr_time<<endl;

	}
	output<<"--------所有作业调度完毕------\n";
	output<<"平均等待时间: "<<sum1/num<<"\n平均周转时间: "<<sum2/num<<"\n平均带群周转时间: "<<sum3/num;
	initial_jobs_again();
}
/*******************************************************************************************************/
// p 进程数，r资源种类
int p ;
int r ;
int maxs[10][10]; //最大需求矩阵
int allocation[10][10]; //分配矩阵
int need[10][10];   //需求矩阵
int available[10]; //可用资源向量
int request[10];   //请求向量当前进程对各类资源的申请量,算法的入口参数

//输入函数，从文件中读取数据集
void infInput()
{
	int m,n;
	fstream input("data/in/data_bank.txt");
	input >> n >> m;
	p=n;r=m;
	cout << "进程数\t" << n << "\n资源数\t" << m << endl;

	cout << "max矩阵为：" << endl;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			input >> maxs[i][j];
			cout << maxs[i][j] << "\t";
		}
		cout << endl;
	}
	cout << "alloc矩阵为：" << endl;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			input >> allocation[i][j];
			cout << allocation[i][j] << "\t";
		}
		cout << endl;
	}
	cout << "need矩阵为：" << endl;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			input >> need[i][j];
			cout << need[i][j] << "\t";
		}
		cout << endl;
	}
	cout << "available矩阵为：" << endl;
	for (int i = 0; i < m; i++) {
		input >> available[i];
		cout << available[i] << " ";
	}
//	cout<<"\n---------------------------------------------------------------"<<endl;
}

//比较函数
//比较进程为m中的元素全大于n中的元素返回1，否则返回0
int compare(int m[],int n[])
{
    int i;
    for(i=0; i<r; i++)
    {
        if(m[i]<n[i])
        {
            return 0;
        }
    }
    return 1;
}


//安全性检验函数，检测是否存在安全序列
int stest()
{
    int i,j,k,l,flag=0;
    int finish[p];
    int work[r];
    for(i=0; i<p; i++)
    {
        finish[i]=0;
        //vis为1即表示available满足第i进程的资源需要
    }
    for(i=0; i<r; i++)
    {
        work[i]=available[i];
    }
    cout<<"分配序列：\n";
    cout<<"            allocation            need              avilable"<<endl;
    for(k=0; k<p; k++)
    {
        for(i=0; i<p; i++)
        {
            if(finish[i]==1)
            {
                continue;
            }
            else
            {
                if(compare(work,need[i]))//available>=need
                {
                    finish[i]=1;
                    cout<<'\n'<<"进程"<<i+1<<'\t';
                    flag=1;
                    for (j =0; j<r; j++)
                    {
                        printf("  %2d ", allocation[i][j]);
                    }
                    cout<<"     ";
                    for (j = 0; j < r; j++)
                    {
                        printf("  %2d ", need[i][j]);
                    }
                    cout<<"     ";
                    for (j = 0; j <r; j++)
                    {
                        printf("  %2d ", work[j] +allocation[i][j]);
                    }
                    for(l=0; l<r; l++)
                    {
                        work[l]=work[l]+allocation[i][l];
                        //进程完成，释放资源
                    }
                    break;
                }
            }
            if(flag==1)
            {
                break;
            }
        }
    }
    cout<<"\n\n";
    for(l=0; l<p; l++)
    {
        if(finish[l]==0)
        {
            return 0;//不存在安全序列
        }
    }
    return 1;//存在安全序列
}

//申请进程后的安全性检验函数

void rtest(int n)
{
    int j;
    //n=n-1;
    if(compare(available,request)&&compare(need[n-1],request))//available>=request 并且 need >=request
    {
        for(j=0; j<r; j++)
        {
            allocation[n-1][j]=allocation[n-1][j]+request[j];
            need[n-1][j]=need[n-1][j]-request[j];
            available[j]=available[j]-request[j];
        }
        if(stest())
        {
            cout<<"允许"<<n<<"进程申请资源！\n";
        }
        else
        {
            cout<<"不允许"<<n<<"进程申请资源！\n";
            for(j=0; j<r; j++)
            {
                allocation[n-1][j]=allocation[n-1][j]-request[j];
                need[n-1][j]=need[n-1][j]+request[j];
                available[j]=available[j]+request[j];
            }
        }
    }
    else
    {
        cout<<"申请资源量越界！\n";
    }
}

void Bank()
{
    int i,n;       //n-第n个资源申请
    infInput();//输入函数

	cout<<"\n---------------------------------------------------------------"<<endl;
    if(stest()==1)
    {
        cout<<"存在安全序列，初始状态安全。\n";
    }
    else
    {
        cout<<"不存在安全序列，初始状态不安全。\n";
    }
	cout<<"---------------------------------------------------------------"<<endl;

    while(1){
    	cout<<"请输入发出请求向量request的进程编号：";
		cin>>n;
		cout<<"请输入请求向量request: ";
		for(i=0; i<r; i++)
		{
			cin>>request[i];
		}
		rtest(n);
		cout<<endl;
    }
}

/*******************************************************************************************************/
#define MAX 9999
typedef struct {
	int id;
	int roundTime;
	int runTime;

	int endTime;
	int finTime;;
}process;
process tasks[2];

void getData(){
	fstream input("data/in/data_LLF.txt");
	input>>tasks[0].id>>tasks[0].roundTime>>tasks[0].runTime;
	input>>tasks[1].id>>tasks[1].roundTime>>tasks[1].runTime;
	tasks[0].endTime=tasks[0].finTime=0;
	tasks[1].endTime=tasks[1].finTime=0;
}

int getDeadEarlyLine(int deadTime,int runTime,int finTime){
	return deadTime-(runTime-finTime);
}

void EDLJob(){
	int del[2];
	fstream output("data/out/7.txt");
	output<<"进程标号\t进程周期\t执行时间\t截止时间\t完成时间"<<endl;
	output<<tasks[0].id<<"\t\t"<<tasks[0].roundTime<<"\t\t"<<tasks[0].runTime<<"\t\t"<<tasks[0].endTime<<"\t\t"<<tasks[0].finTime<<endl;
	output<<tasks[1].id<<"\t\t"<<tasks[1].roundTime<<"\t\t"<<tasks[1].runTime<<"\t\t"<<tasks[1].endTime<<"\t\t"<<tasks[1].finTime<<endl;

	for(int i=0;i<100;i++){
		if(i%20==0){
			tasks[0].endTime=i+tasks[0].roundTime;//每20一个周期
			tasks[0].finTime=0;//新周期已完成设为0
		}
		if(i%50==0){
			tasks[1].endTime=i+tasks[1].roundTime;//每50一个周期
			tasks[1].finTime=0;//新周期已完成设为0
		}

		for(int j=0;j<2;j++){
			if(tasks[j].finTime>=tasks[j].runTime){
				del[j]=MAX;//当完成时，最早截至时间设为MAX
			}
			else
				del[j]=getDeadEarlyLine(tasks[j].endTime,tasks[j].runTime,tasks[j].finTime);
		}

		if(i%10==0) output<<"\n";
		else{
			if(del[0]<=del[1]){
				output<<i<<"---a\t";
				tasks[0].finTime++;
			}
			else{
				output<<i<<"---b\t";
				tasks[1].finTime++;
			}
		}

	}
}

void EDL(){
	getData();
	EDLJob();
}
/*******************************************************************************************************/

//进程结构体
typedef struct Task //进程
{
	char pname[5]; //进程名
	int deadtime; //周期
	int servetime; //执行时间
//周期进程某一次执行到停止的剩余需执行时间（考虑到抢占），初始为deadtime
	int lefttime;
	int cycle; //执行到的周期数
//进程最近一次的最迟开始执行时间，- currenttime 即为松弛度
	int latestarttime;
//进程下一次最早开始时间
	int arivetime;
	int k; //k=1，表示进程正在运行，否则为0，表示进程不在执行期间
	/*
	 若存在最小松弛度进程个数多于1个，
	 则采用最近最久未使用算法
	 采用一计数器LRU_t
	 */
	int LRU_t;
} Task;

typedef struct sqqueue //循环队列
{
	Task *data[queuesize];
	int front, rear;
} sqqueue;

//初始化n个进程
void InitTask(Task *pro, int n) {
	int i;
	Task *p = pro;
	for (i = 0; i < n; i++) {
		*(p->pname) = '\0';
		p->deadtime = 0;
		p->servetime = 0;
		p->lefttime = 0;
		p->cycle = 0;
		p->latestarttime = 0;
		p->arivetime = 0;
		p->k = 0;
		p->LRU_t = 0;
		p++;
	}
}

//初始化队列（队空）
void InitQueue(sqqueue *que) {
	que->front = que->rear = 0;
}

//进程进入循环队列
void enterQueue(sqqueue *que, Task *pro) {
//判断循环队列是否已满
	if ((que->rear + 1) % queuesize == que->front)
		printf("队列已满!\n");
	else {
		que->data[que->rear] = pro; //进程放入队尾
		que->rear = (que->rear + 1) % queuesize; //队尾指针加1
		printf("%s成功入队!\n", pro->pname); //显示进程入队
	}
}

//从当前队列中找到最低松弛度的进程
Task* llf(sqqueue *dui, int currenttime) {
	sqqueue *q1 = dui;
	Task *currentpro, *pro;

	int minllf = MINLLF, llf;
	int pro_LRU_t = PRO_LRU_T;
	int front = q1->front, rear = q1->rear; //队首元素？？

//将队首进程赋给当前进程
	currentpro = q1->data[front];

	if (currenttime <= MAXTIME) {
//求最短松弛度currentpro，如果队列中只有一个进程
		if (front == rear) {
			return currentpro;
			printf("%dms时%s%d进程的松弛度为：%d\n", currenttime, currentpro->pname,
					currentpro->cycle, llf);
		}

//进程数目多于一个
		else {
			/*
			 找当前时间下可以开始执行的进程中松弛度最小的，
			 赋给currentpro.
			 当最小松弛度的进程多于1个，
			 我们采用的是最后1个最小松弛度进程?
			 */
			do {
				if (front != rear) {
					pro = q1->data[front];
					if (pro->arivetime <= currenttime && currenttime <= MAXTIME) {
//计算松弛度 = 周期*需执行的次数- （上一次）执行的时间 -当前时间
						llf = (pro->deadtime) * (pro->cycle) - pro->lefttime
								- currenttime;
						printf("%dms时%s%d进程的松弛度为：%d\n", currenttime, pro->pname,
								pro->cycle, llf);
						if (minllf >= llf) //比较得出最低松弛度
								{

							if (pro->LRU_t >= pro_LRU_t) {
								pro_LRU_t = pro->LRU_t;
								minllf = llf;
								currentpro = pro;
							}

						}
					}
					front = (front + 1) % queuesize;
				} else
					break;
			} while (front != rear); //检测队列是否遍历完

		}

	}
	return currentpro;
}

//寻找松弛度 <=0 的抢占进程，替代当前最小松弛度进程
Task* leastlaxityfirst(sqqueue *dui, int currenttime) {
	sqqueue *q1 = dui;
	Task *pro = NULL, *nextpro;
	int front = q1->front, rear = q1->rear;

	/*
	 当队列不空，寻找当前时刻
	 是否有松弛度为0（需抢占进程）的进程
	 */
	while (front != rear) {
		nextpro = q1->data[front];
		/*
		 pro->latestarttime初始为：
		 (pro->deadtime)*(pro->cycle) - pro->servetime;
		 pro->latestarttime - currenttime 即为松弛度
		 pro->latestarttime - currenttime <= 0 ,
		 即松弛度 = 0，抢占，跳出循环
		 */
		if (nextpro->latestarttime <= currenttime)
			break;
		else
			front = (front + 1) % queuesize;
	}
//如果队列空，返回pro
	if (front == rear)
		return pro;
//队列不空，nextpro为此时抢占正在执行的进程的进程
	else
		return nextpro;
}

//从队列中读取进程的过程
void LLF_Task(sqqueue *dui) {
	int currenttime = 0;

	sqqueue *que = dui;
	int front = que->front;
	int rear = que->rear;

	Task *currentpro, *pro, *tmppro;

//currentpro为当前时间队列中松弛度最低的进程
	currentpro = llf(que, currenttime);

//在MAXTIME时间内考虑
	while (currenttime <= MAXTIME) {
		/*
		 最低松弛度进程为空，
		 即为当前时间没有可运行的进程
		 */
		if (currentpro == NULL) {
			printf("%dms时无可运行进程!\n", currenttime);
			break;
		} else {
			if ((currentpro->arivetime <= currenttime)) {
				if (currenttime == MAXTIME) {
//当进程正在运行
					if (currentpro->k == 1)
						printf("%d ms: %s%d\n\n", currenttime,
								currentpro->pname, currentpro->cycle);

//当此时没有进程运行
					else
						printf("\n=======执行只考虑前%dms，现时间已到%dms了！======\n",
								currenttime, currenttime);
					break;
				} else if (currenttime != MAXTIME) {
					if (currentpro->k == 0) {
						printf("=> %dms时刻%s%d进程开始执行: %d - ", currenttime,
								currentpro->pname, currentpro->cycle,
								currenttime);
						currentpro->k = 1; //表明进程开始运行

						do {
							if (front != rear) {
								pro = que->data[front];
								if (pro != currentpro)
									pro->LRU_t++;
								front = (front + 1) % queuesize;
							} else
								break;
						} while (front != rear);
					}
					currenttime++; //当前时间增加
					currentpro->lefttime--; //运行剩余时间减少

//当剩余运行时间等于0, 即当程序运行结束
					if (currentpro->lefttime == 0) {
						if (currentpro->k == 1) {
							printf("%d ms: %s%d\n\n", currenttime,
									currentpro->pname, currentpro->cycle);
							currentpro->k = 0; //表明进程开始进入不执行状态
						}
						currentpro->cycle++;
						currentpro->lefttime = currentpro->servetime;
						currentpro->arivetime = (currentpro->deadtime)
								* (currentpro->cycle - 1);
						currentpro->latestarttime = (currentpro->deadtime)
								* (currentpro->cycle) - (currentpro->servetime);

						currentpro = llf(que, currenttime);
					}

//当进程未运行完毕。。。（可能被抢占）
					else {
//pro为抢占当前正执行进程的进程
						pro = leastlaxityfirst(que, currenttime);
						if (pro != NULL) {
							/*
							 如果当前存在抢占进程，
							 即pro != currentpro，
							 则使currentpro进程进入不执行状态
							 */
							if (pro != currentpro) {
								if (currentpro->k == 1)
									currentpro->k = 0;
								printf("%d ms: %s%d\n\n", currenttime,
										currentpro->pname, currentpro->cycle);
								printf("%dms时%s%d进程被进程%s%d进程抢占！\n", currenttime,
										currentpro->pname, currentpro->cycle,
										pro->pname, pro->cycle);
							}
							/*
							 使currentpro为松弛度最小的进程，
							 不论是否是抢占程序。
							 */
							currentpro = pro;
						}
					}
				}
			}

//当进程下一次开始时间还没到
			else if (currentpro->arivetime >= currenttime) {
				if (currenttime == MAXTIME) {
					printf("\n=======执行只考虑前%dms，现时间已到%dms了！======\n",
							currenttime, currenttime);
					break;
				} else {
					printf("第%dms时没有进程到达!\n");
					currenttime++;
					currentpro = llf(que, currenttime);
				}
			}
		}
	}
}

void LLF() {
	sqqueue *dui, *dui2;
	Task *pro, pro2[queuesize], *pro3;

	int front, rear, ci = 0, pi = 0;
	int flag = 1, i;
	char ch, ch2, name[5];

	printf("\n*******最低松弛调度**********\n\n");

	dui = (sqqueue*) malloc(sizeof(sqqueue));
	dui->rear = dui->front = 0;
	dui2 = (sqqueue*) malloc(sizeof(sqqueue));
	dui2->rear = dui2->front = 0;

	i = 0;
	InitQueue(dui);
	InitTask(pro2, queuesize);

	fstream input("data/in/data_LLF.txt");
	for(int k=0;k<2;k++) {
		pro = pro2 + i;

		input>>pro->pname>>pro->deadtime>>pro->servetime;
		pro->lefttime = pro->servetime; //
		pro->cycle = 1; //初始时进程从第一周期开始执行
		pro->latestarttime = (pro->deadtime) * (pro->cycle)
				- pro->servetime; //进程下一次最迟开始执行的时间
		pro->arivetime = (pro->deadtime) * (pro->cycle - 1); // 进程下一次开始的最早时间
		pro->k = 0;
		enterQueue(dui, pro); //进队列
		i++; //进程个数
	}

	dui2 = dui;
	front = dui2->front;
	rear = dui2->rear;

	while (front != rear) {
		pro3 = dui2->data[front];
		ci = pro3->servetime + ci; //各进程执行的时间总和
		pi = pro3->deadtime + pi; //各周期总和
		front = (front + 1) % queuesize;
	}
	LLF_Task(dui);
}
/*******************************************************************************************************/



//主函数
int main() {
	initial_jobs(); //初始化所有作业信息
	read_Jobdata(); //读取数据文件
	int choice = 1;
	while (choice != 0) {
		cout<<"------------------"<<endl;
		cout<<"(1)FCFS	先来先服务"<<endl;
		cout<<"(2)SJF	短作业优先"<<endl;
		cout<<"(3)HRRN	高响应比优先"<<endl;
		cout<<"(4)PSA	优先级调度"<<endl;
		cout<<"(5)RR	时间片轮转"<<endl;
		cout<<"(6)CRT	最短剩余时间"<<endl;
		cout<<"(7)EDL	最早截止时间"<<endl;
		cout<<"(8)LLF	最底松弛度"<<endl;
		cout<<"(9)---	银行家算法"<<endl;
		cout<<"(0)退出."<<endl;
		cout<<"请选择调度算法：	";
		cin>>choice;

		switch (choice) {
		case 1:
			FCFS();
			cout<<endl;
			break;
		case 2:
			SJF();
			cout<<endl;
			break;
		case 3:
			HRRF();
			cout<<endl;
			break;
		case 4:
			PSA();
			cout<<endl;
			break;
		case 5:
			RR();
			cout<<endl;
			break;
		case 6:
			CRT();
			cout<<endl;
			break;
		case 7:
			EDL();
			break;
		case 8:
			LLF();
			break;
		case 9:
			Bank();
			break;
		case 0:
			printf("退出程序\n");
			break;
		}
	}
}
