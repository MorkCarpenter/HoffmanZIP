#pragma once
#include <string>
#include <vector>
#include <stack>
#include <algorithm>
namespace hfm {
	using bit8 =	  unsigned char;
	using  ULL = unsigned long long;
	struct Statistic {
		bit8 character = '\0';
		ULL     weight =    0;
	};
	struct Node {
		bit8   character = '\0';
		ULL		  weight =    0;
		std::string code =   "";
		int       parent =    0;
		int   left_child =    0;
		int  right_child =    0;
	};
	bool StatisticCMP(const Statistic& a, const Statistic& b);
	//解压
	int extract(const std::string& ifile_name);
	//解压
	int extract(const std::string& ifile_name,
				const std::string& ofile_name);
	//压缩
	int compress(const std::string& ifile_name);
	//压缩
	int compress(const std::string& ifile_name,
				 const std::string& ofile_name);

	class HFMZip {
	public:
		HFMZip();
		~HFMZip();
		//选出最小的两个节点
		void Select(const size_t& end_ptr,
			int& min_node,
			int& penul_node);
		void CreateTree(void);
		void CreateCode(void);
		//返回-2：输出文件失败
		//返回-1：读取文件失败
		//返回 0：函数执行正常
		int LoadFile(const std::string& ifile_name,
					 const std::string& ofile_name);
		//返回-2：输出文件失败
		//返回-1：读取文件失败
		//返回 0：函数执行正常
		int CreateFile(const std::string& ifile_name,
					   const std::string& ofile_name);
	private:
		//统计文件里的字符
		std::vector<Statistic> fileio;
		//哈夫曼树
		std::vector<Node> tree;
		//字符数
		size_t char_kind;
		//节点数
		size_t node_number;
		//哈夫曼编码
		std::vector<std::string> code;
		//文件读取操作
		FILE* infile;
		//文件输出操作
		FILE* outfile;
	};
}

