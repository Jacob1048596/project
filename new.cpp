#include<string>
#include<vector>
#include<iostream>
using namespace std;


string MaxNum(string num,vector<int> change)
{
	for(int i=0;i<num.size();i++){
		if(num[i]-'0'<change[num[i]-'0'])
		{
			while(i<num.size()&&num[i]-'0'<change[num[i]-'0']){
				num[i]=change[num[i]-'0']+'0';
				i++;
			}
			break;
		}	
	}
	return num;
}

int main(){
	string num="012";
	vector<int> change={9,8,5,0,3,6,4,2,6,8};
	cout<<MaxNum(num,change);

}