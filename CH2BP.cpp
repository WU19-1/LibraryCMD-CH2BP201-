#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<iostream>
#include<fstream>
#define NDEBUG
#include<assert.h>
#include<dirent.h>
#include<errno.h>
#include<unistd.h>

using namespace std;

char pref[50] = "";

struct Genre{
	string name;
	struct Book* books;
}*head[15],*tail[15];

struct DetailBook{
	string isbn;
	string title;
	int years;
	string publisher;
	string author;
	
	DetailBook* factory(string isbn,string title,int years,string publsiher,string author){
		DetailBook *db = new DetailBook;
		db->isbn = isbn;
		db->title = title;
		db->publisher = publisher;
		db->author = author;
		db->years = years;
		return db;
	}
};

struct Book{
	DetailBook *details;
	Book* books[255];
};

Book *factory(){
	Book *b = new Book;
	for(int i = 0 ; i < 256 ; i++){
		b->books[i] = NULL;
	}
	b->details = NULL;
	return b;
}

int hasher(char* str){
	unsigned long res = 5381;
	int c;
	while(c = *str++)
		res = ((res << 5) + res) + c;
	return res % 15;
}

void insert_genre(char* str){
	int idx = hasher(str);
	while(head[idx]){
		idx += hasher(str);
		idx %= 15;
	}
	Genre *g = new Genre;
	g->name = str;
	g->books = factory();
	head[idx] = tail[idx] = g;
}

bool get_genre(char *str){
	int idx = hasher(str);
	string xx = "";
	for(int i = 0 ; i < strlen(str); i++){
		xx.push_back(str[i]);
	}
	if(head[idx] == NULL) return false;
	while(head[idx]->name.compare(xx) != 0 && head[idx] != NULL){
		idx += hasher(str);
		idx %= 15;
	}
	if(head[idx] != NULL) return true;
	return false;
}

Genre* get_genres(char *str){
	int idx = hasher(str);
	while(head[idx]->name.compare(str) != 0 && head[idx] != NULL){
		idx += hasher(str);
		idx %= 15;
	}
	if(head[idx]->name.compare(str) == 0 && head[idx] != NULL){
		return head[idx];
	}
	else{
		return NULL;
	}
}

bool check_isbn(string isbn){
	for(int i = 0 ; i < isbn.length() ; i++){
		if(isbn[i] < '0' || isbn[i] > '9') return false;
	}
	return true;
}

bool check_publisher(string publisher){
	string end = "company";
	return (0 == publisher.compare(publisher.length() - end.length(),end.length(),end));
}

void insert_books(Book *books , DetailBook *db){
	Book *crawl = books;
	for(int i = 0 ; i < db->title.length(); i++){
		if(crawl->books[db->title[i]] == NULL){
			crawl->books[db->title[i]] = factory();
		}
		crawl = crawl->books[db->title[i]];
	}
	crawl->details = db;
}

bool print_detail(Book *b){
	puts("Found the book!");
	cout << "Title : " << b->details->title << endl;
	cout << "Author : " << b->details->author << endl;
	cout << "ISBN : " << b->details->isbn << endl;
	cout << "Publisher : " << b->details->publisher << endl;
	cout << "Years : " << b->details->years << endl;
	
	bool end_book = true;
	
	for(int i = 0 ; i < 256 ; i++){
		if(b->books[i] != NULL) end_book = false;
	}
	return end_book;
}

void find_books(Book *books , string name){
	if(books->details != NULL){
		if(name.compare(pref) != 0){
			printf("-. ");
			cout << books->details->title << endl;
		}
	}
	
	for(int i = 0 ; i < 256; i++){
		if(books->books[i]){
			name.push_back(i);
			find_books(books->books[i],name);
			name.pop_back();
		}
	}
}

DetailBook* process_file(string all){
	char arr[all.length()] = "";
	for(int i = 0 ; i < all.length(); i++){
		arr[i] = all[i];
	}
	char isbn[100] = "";
	char title[100] = "";
	int years = 0;
	char publisher[100] = "";
	char author[100] = "";
	sscanf(arr,"%[^#]#%[^#]#%[^#]#%[^#]#%d",title,author,isbn,publisher,&years);
	DetailBook *db = new DetailBook;
	db->author = string(author);
	db->isbn = string(isbn);
	db->publisher = string(publisher);
	db->title = string(title);
	db->years = years;
	return db;
}

void find_sub(Book *books,string substr,char *x){
	if(books->details != NULL){
		size_t found = substr.find(x);
		if(found != string::npos && substr.compare("") != 0){
			cout << substr << endl;
		}
	}
	for(int i = 0 ; i < 256; i++){
		if(books->books[i]){
			substr.push_back(i);
			find_sub(books->books[i],substr,x);
			substr.pop_back();
		}
	}
}

int main(int argc, char **argv){
	int count = 0;
	string isbn;
	string title;
	int years;
	string publisher;
	string author;
	char arr[50] = "";
	char genrepath[50] = "";
	FILE *genre_file;
	FILE *fp = fopen("genre.txt","r");
	if(!fp){
		puts("File genre.txt is generated");
		fopen("genre.txt","a");
		fp = fopen("genre.txt","r");
	}
	
	DIR *dir = opendir("genres");
	
	if(errno == ENOENT){
		mkdir("genres");
		puts("Successfully generate \'genres\' folder");
	}
	
	ifstream myfile;
	
	while(!feof(fp)){
		string all;
		fscanf(fp,"%[^\n]\n",arr);
		insert_genre(arr);
		Genre *ge = get_genres(arr);
		sprintf(genrepath,"genres/%s",arr);
		string path = "genres/";
		for(int i = 0 ; i < strlen(arr); i++){
			path.push_back(arr[i]);
		}
		
		myfile.open(path);
		
		if(myfile.is_open()){
			while(!myfile.eof()){
				getline(myfile,all);
				DetailBook *db = process_file(all);
				insert_books(ge->books,db);
			}
			myfile.close();
		}
		count++;
	}
	
	if(strcmp(argv[1],"-help") == 0){
		puts("cmd library");
		puts("created by : WU19-1");
		puts("CH2BP.exe command");
		puts("");
		puts("SYNOPSIS");
		puts("CH2BP [options]");
		puts("");
		puts("Options : ");
		puts("\t1.  -i type");
		puts("\t\t-i is used for inserting into the file");
		puts("\t\ttype is the type that you want to insert");
		puts("\t\ttype :");
		puts("\t\tgenre for inserting genre");
		puts("\t\tbook for inserting book");
		puts("\t2.  -s [genre] name");
		puts("\t\t-s is used for searching book\'s name");
		puts("\t\tgenre is used to specify a specific genre of the book");
		puts("\t\tthis input is optional, if you doesn\'t input the specific genre");
		puts("\t\tthen the program will search the book in all genre");
		puts("\t\tname is the name of the book that you want to search");
		puts("\t\tyou can input it specifically (case sensitive)");
		puts("\t\tor you can input the prefix(partial words of the book\'s title)");
		puts("\t\tso the program can autocompleted the result");
		puts("\t3.  -sub prefix");
		puts("\t\t-sub parameter is used to search book in all genres with prefix that you passed");
		puts("\t\tif the book\'s title contains the prefix that you passed, then");
		puts("\t\tthis program will autocompleted the search");
		puts("\t4.  -help");
		puts("\t\tview this manual.");
	}
	
	else if(strcmp(argv[1],"-i") == 0){
		if(strcmp(argv[2],"genre") == 0){
			if(count >= 15){
				puts("Maximum genre is reach!");
			}else{
				do{
					strcpy(arr,"");
					printf("Insert genre name[5 - 50 characters] : ");
					scanf("%[^\n]",arr);
					getchar();
				}while(strlen(arr) < 5 || strlen(arr) > 50);
				insert_genre(arr);
				fp = fopen("genre.txt","a");
				fprintf(fp,"%s\n",arr);
			}
		}else if(strcmp(argv[2],"book") == 0){
			rewind(fp);
			fseek(fp,0,SEEK_END);
			int size = ftell(fp);
			if(size == 0){
				cout << "There are no available genre!" << endl;
			}else{
				rewind(fp);
				fp = fopen("genre.txt","rb");
				while(!feof(fp)){
					fscanf(fp,"%[^\n]\n",arr);
					cout << "-. " <<arr << endl;
				}
				do{
					cout << "Insert book genre : ";
					cin >> arr;
					getchar();
				}while(!get_genre(arr));
				
				do{
					cout << "Insert book title [5-50] : ";
					getline(cin,title);
				}while(title.length() < 5 || title.length() > 50);
				
				do{
					cout << "Insert isbn [13 numbers] : ";
					getline(cin,isbn);
					if(isbn.length() < 13 || isbn.length() > 13 || !check_isbn(isbn)) continue;
					else break;
				}while(true);
				
				do{
					cout << "Insert author name [5-50] : ";
					getline(cin,author);
				}while(author.length() < 5 || author.length() > 50);
				
				do{
					cout << "Insert publisher name [ends with \'company\'] : ";
					getline(cin,publisher);
					if(publisher.length() < 7) continue;
					if(check_publisher(publisher)) break;
				}while(true);
				
				do{
					cout << "Insert the year the book\'s were published [1800 - 2019] : ";
					cin >> years;
				}while(years < 1800 || years > 2019);
				Genre *g = get_genres(arr);
				char path[50] = "";
				sprintf(path,"genres/%s",arr);				
				genre_file = fopen(path,"a");
				DetailBook *db = new DetailBook;
				db->author = author;
				db->isbn = isbn;
				db->publisher = publisher;
				db->years = years;
				db->title = title;
				insert_books(g->books,db);
				ofstream cout(path);
				cout << db->title << "#";
				cout << db->author << "#";
				cout << db->isbn << "#";
				cout << db->publisher << "#";
				cout << db->years << endl;
			}
		}
	}
	
	else if(strcmp(argv[1],"-s") == 0){
		if(argv[2] == NULL || strcmp(argv[2],"") == 0){
			puts("Please input the correct parameter");
			puts("Check -help for more information");
			exit(0);
		}
		bool checking = false;
		if(argc == 3){
			puts("All search");
			puts(argv[3]);
			if(strcmp(argv[2],"") == 0){
				puts("Please input paramaters \'name\'");
				exit(0);
			}
			//search all
			strcpy(pref,argv[2]);
			for(int i = 0 ; i < 16 ; i++){
				checking = false;
				if(head[i] != NULL){
					Book *find_all = head[i]->books;
					for(int i = 0 ; i < strlen(argv[2]); i++){
						if(find_all->books[argv[2][i]]){
							find_all = find_all->books[argv[2][i]];
						}else{
							checking = true;
							break;
						}
					}
					if(checking) continue;
					else {
						if(find_all->details != NULL){
							print_detail(find_all);
						}
					}
				}
			}
			puts("Did you mean : ");
			for(int i = 0 ; i < 16 ; i++){
				checking = false;
				Book *find_all;
				if(head[i] != NULL){
					find_all = head[i]->books;
					for(int i = 0 ; i < strlen(argv[2]); i++){
						if(find_all->books[argv[2][i]]){
							find_all = find_all->books[argv[2][i]];
						}else{
							checking = true;
							break;
						}
					}
					if(checking) continue;
					else {
						find_books(find_all,argv[2]);
					}
				}
			}
		}else if(argc == 4){
			if(argv[3] == NULL){
				puts("Please input paramaters \'name\'");
				exit(0);
			}
			//search specific
			puts("Specific search");
			assert(head[hasher(argv[3])] != NULL);
			Book *find_all = head[hasher(argv[2])]->books;
			for(int i = 0 ; i < strlen(argv[3]); i++){
				if(find_all->books[argv[3][i]]){
					find_all = find_all->books[argv[3][i]];
				}else{
					puts("No such books exists in the genre!");
					exit(0);
				}
			}
			if(find_all->details != NULL){
				print_detail(find_all);
			}
			strcpy(pref,argv[3]);
			find_books(find_all,argv[3]);
		}
	}
	else if(strcmp(argv[1],"-sub") == 0){
		string empty = "";
		if(argv[2] == NULL){
			puts("Please input [search] parameter");
			exit(0);
		}
		else if(empty.compare(argv[2]) == 0){
			puts("[search] parameter cannot be empty!");
			exit(0);
		}
		puts("Substring result : ");
		for(int i = 0 ; i < 16 ; i++){
			Book *find_subs;
			if(head[i] != NULL){
				find_subs = head[i]->books;
				find_sub(find_subs,empty,argv[2]);
			}else continue;
		}
	}
	return 0;
}
