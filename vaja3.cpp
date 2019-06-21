#include <iostream>
#include <fstream>
#include <streambuf>
#include <vector>
using namespace std;

struct znak{
	char znak=' ';
	int st=0;
	vector<bool> biti;
};

class BinWriter {
public:
	int k;
	ofstream f;
	char x;

	BinWriter(const char *p) : k(0) {
		f.open(p, ios::binary);
	}

	~BinWriter() {
		if (k > 0) writeByte(x);
		f.close();
	}

	void writeByte(char x) {
		f.write((char*)&x, 1);
	}

	void writeInt(int y) {
		f.write((char*)&y, 4);
	}

	void writeBit(bool b) {
		if (k == 8) {
			writeByte(x);
			k = 0;
		}
		x ^= (-b ^ x) & (1 << k);
		k++;
	}
};

class BinReader {
public:
	int k, i;
	ifstream f;
	char x;

	BinReader(const char *p) : k(0) {
		f.open(p, ios::binary);
	}

	char readByte() {
		f.read((char*)&x, 1);
		return x;
	}

	int readInt() {
		f.read((char*)&i, 4);
		return i;
	}

	bool readBit() {
		if (k == 8) {
			readByte();
			k = 0;
		}
		bool b = (x >> k) & 1;
		k++;
		return b;
	}
};

//prototipi funkcij
void stiskanje(znak*, string);
void iskanjeRazdelitve(znak*, int, int);
void vpisi(znak*, string, unsigned int);
void dekodiranje(znak*);
int stVsehBitov(znak*);

int main(int argc, char* argv[]) {
	/*BinReader br("test.bin");
	br.readByte();
	for (int i = 0; i < 8; ++i)
		std::cout << (int)br.readBit() << "\n";
	br.f.close();*/
	if(argv[1][0] == 'c'){
			//branje datoteke	
			std::ifstream t("file.txt");
			std::string str;
			t.seekg(0, std::ios::end);   
			str.reserve(t.tellg());
			t.seekg(0, std::ios::beg);
			str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

			znak znaki[256];
			stiskanje(znaki, str);

			//vpisovanje v datoteko
			vpisi(znaki, str, str.length());
	}else if(argv[1][0] == 'd'){
			znak znaki[256];
			dekodiranje(znaki);
	}else{
		cout<<"Vnešena nepravilna zastavica\n";
	}

	return 0;
}

void stiskanje(znak *znaki, string str){
	//kreiranje tabele
	for(unsigned int x=0; x<str.length(); x++){
		znaki[(int)str[x]].st++;
		znaki[(int)str[x]].znak=str[x];
	}

	//urejanje po velikosti
	for(int x=0; x<255; x++){
		for(int i=x; i<255; i++){
			if(znaki[x].st<znaki[i].st){
				znak vmes = znaki[x];
				znaki[x]=znaki[i];
				znaki[i]=vmes;	
			}
		}
	}
	//for(int x=0;x<13;x++){
	//	cout<<znaki[x].st<<endl<<znaki[x].znak<<endl;
	//}
	
	iskanjeRazdelitve(znaki, 255, 0);

	//izpis bitov
	//for(int x=0; x<12; x++){
	//	cout<<znaki[x].znak<<": ";
	//	for (std::vector<bool>::iterator it = znaki[x].biti.begin() ; it != znaki[x].biti.end(); ++it) std::cout << *it;
	//cout<<endl;
	//}
}

void iskanjeRazdelitve(znak *znaki, int stEle, int x){
	if(stEle!=x){
		int leva=x, desna=stEle;
		int sestLevo=0, sestDesno=0; 
		sestLevo = znaki[x].st;
		sestDesno = znaki[stEle].st;
	
		while(x+1!=stEle && stEle-1!=x){
			if(sestLevo > sestDesno){
				stEle--;
				sestDesno+=znaki[stEle].st;	
			}else{
				x++;
				sestLevo+=znaki[x].st;
			}
		}
		//cout<<"\nlevo: "<<sestLevo<<"\ndesno: "<<sestDesno<<endl;
		//cout<<"x: "<<x<<"\nstEle: "<<stEle<<endl;

		//vstavljanje bitov
		for(int i=leva; i<=x && znaki[i].st!=0; i++) znaki[i].biti.push_back(0);
		for(int i=stEle; i<=desna && znaki[i].st!=0; i++) znaki[i].biti.push_back(1);
	
		iskanjeRazdelitve(znaki, x, leva);
		iskanjeRazdelitve(znaki, desna, stEle);
	}
}

void vpisi(znak *znaki, string str, unsigned int dol){
	BinWriter bw("out.bin");
	for(int x=0; x<256; x++){
		bw.writeByte(znaki[x].znak);
		bw.writeInt(znaki[x].st);
	}
	int iskani;
	//cout<<str<<endl;
	for(unsigned int x=0; x<dol; x++){
		//cout<<"str: "<<str[x]<<endl;
		for(iskani=0; str[x]!=znaki[iskani].znak; iskani++);
		//cout<<"znak: "<<znaki[iskani].znak<<endl<<"Vpisano: ";
		for (std::vector<bool>::iterator it = znaki[iskani].biti.begin() ; it != znaki[iskani].biti.end(); ++it){
			bw.writeBit(*it);
			//cout<<*it;
		}
		//cout<<endl;
	}
	bw.f.close();
}

void dekodiranje(znak* znaki){
	BinReader br("out.bin");
	for(int x=0; x<256; x++){
		znaki[x].znak = br.readByte();
		znaki[x].st = br.readInt();
	}
	//br.f.close();

	iskanjeRazdelitve(znaki, 255, 0);
	
	//sortiranje
	for(int x=0; x<256; x++){
		for(int i=x; i<256; i++){
			if(znaki[x].biti.size() < znaki[i].biti.size()){
				znak vmes = znaki[x];
				znaki[x] = znaki[i];
				znaki[i] = vmes;
			}
		}
	}

	//for(int x=0; x<256 && znaki[x].st!=0; x++){
	//	cout<<"znak: "<<znaki[x].znak<<endl;
	//	for (std::vector<bool>::iterator it = znaki[x].biti.begin() ; it != znaki[x].biti.end(); ++it)cout<<*it;
	//	cout<<endl;
	//}
	//cout<<znaki[0].biti.size();
	bool buffer[znaki[0].biti.size()];

	//branje besedila
	br.readByte();

	//napolni buffer
	int stVseh = stVsehBitov(znaki);
	for(unsigned int x=0; x<znaki[0].biti.size(); x++){
		buffer[x]=br.readBit();
		//cout<<buffer[x];
	}
	
	//dekodiranje bitov in sestavljanje datoteke
	bool izpisi;
	for(int i=0, x=znaki[0].biti.size(); x < stVseh; i++){
		int y=0;
		for (std::vector<bool>::iterator it = znaki[i].biti.begin() ; it != znaki[i].biti.end(); ++it){
			if(buffer[y] != *it){
				izpisi = false;
				break;
			}else izpisi = true;
			y++;
		}
		if(izpisi){
			//cout<<"buffer prej: ";
			//for(unsigned int x=0; x<znaki[0].biti.size(); x++)cout<<buffer[x];
			//cout<<endl;

				if(znaki[i].biti.size() == znaki[0].biti.size()){
				for(unsigned int x=0; x<znaki[0].biti.size(); x++){
					buffer[x]=br.readBit();
				}
				x+=znaki[i].biti.size();
			}else{
				//poravnava bufferja
				for(unsigned int por=znaki[i].biti.size(); por<znaki[0].biti.size(); por++){
					buffer[por-znaki[i].biti.size()]=buffer[por];
				}
				
				//poljenje bufferja
				for(unsigned int pol=znaki[0].biti.size()-znaki[i].biti.size(); pol<znaki[0].biti.size(); pol++){
					buffer[pol] = br.readBit();
				}
				x+=znaki[i].biti.size();
			}
			cout<<znaki[i].znak;//<<endl<<"buffer po: ";
			//for(unsigned int x=0; x<znaki[0].biti.size(); x++)cout<<buffer[x];
			//cout<<endl;
			i=0;
		}
	}
}

int stVsehBitov(znak* znaki){
	int sum=0;	
	for(int x=0; x<256 && znaki[x].st!=0; x++)sum+=znaki[x].biti.size()*znaki[x].st;
	return sum;
}
