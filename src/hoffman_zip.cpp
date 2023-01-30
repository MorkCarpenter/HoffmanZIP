#include "hoffman_zip.h"

	hfm::HFMZip::HFMZip() {
		fileio.resize(256);
		for (size_t i = 0; i < 256; ++i) {
			fileio[i].weight = 0;
			fileio[i].character = (bit8)i;
		}
		code.resize(256);
	}
	hfm::HFMZip::~HFMZip() {
	}
	void hfm::HFMZip::Select(const size_t& end_ptr, int& min_node, int& penul_node) {
		ULL min = ULLONG_MAX;
		for (int i = 0; i < end_ptr; ++i) {
			if (tree[i].parent == 0 && min > tree[i].weight) {
				min = tree[i].weight;
				min_node = i;
			}
		}
		tree[min_node].parent = 1;
		min = ULLONG_MAX;
		for (int i = 0; i < end_ptr; ++i) {
			if (tree[i].parent == 0 && min > tree[i].weight) {
				min = tree[i].weight;
				penul_node = i;
			}
		}
	}
	void hfm::HFMZip::CreateTree() {
		int min_node, penul_node;
		for (int i = char_kind; i < node_number; ++i) {
			Select(i, min_node, penul_node);
			tree[i].weight = tree[min_node].weight + tree[penul_node].weight;
			tree[min_node].parent = i;
			tree[penul_node].parent = i;
			tree[i].left_child = min_node;
			tree[i].right_child = penul_node;
		}
	}
	void hfm::HFMZip::CreateCode() {
		size_t left, right, now;
		std::stack<size_t>sk;
		sk.push(tree[node_number - 1].right_child);
		tree[sk.top()].code.insert(0, "1");
		sk.push(tree[node_number - 1].left_child);
		tree[sk.top()].code.insert(0, "0");
		while (!sk.empty()) {
			now = sk.top();
			sk.pop();
			left = tree[now].left_child;
			right = tree[now].right_child;
			if (left != 0 || right != 0) {
				tree[left].code = tree[now].code + '0';
				tree[right].code = tree[now].code + '1';
				sk.push(left);
				sk.push(right);
			}
			else {
				code[tree[now].character] = tree[now].code;
			}
		}
	}
	int hfm::HFMZip::LoadFile(const std::string& ifile_name,
		const std::string& ofile_name) {
		bit8 inchar;
		ULL file_len = 0;
		std::string filekind;
		size_t i = ifile_name.size() - 1;
		while (ifile_name[i] != '.') --i;
		filekind = ifile_name.substr(i);
		fopen_s(&outfile, ofile_name.c_str(), "wb");
		if (outfile == nullptr) return -2;
		fputs(filekind.c_str(), outfile);
		fputc('\1', outfile);
		fopen_s(&infile, ifile_name.c_str(), "rb");
		if (infile == nullptr) return -1;
		do {
			fread(&inchar, sizeof(bit8), 1, infile);
			++fileio[inchar].weight;
			++file_len;
		} while (!feof(infile));
		--fileio[inchar].weight;
		fclose(infile);
		std::sort(fileio.begin(), fileio.end(), StatisticCMP);
		for (i = 0; i < 256; ++i) {
			if (fileio[i].weight == 0) {
				fileio.resize(i + 1);
				fileio.swap(fileio);
				char_kind = fileio.size()-1;
				break;
			}
		}
		if (char_kind == 1) {
			if (infile == nullptr) return -2;
			fwrite((char*)&char_kind, sizeof(size_t), 1, outfile);
			fwrite((char*)&fileio[0].character, sizeof(bit8), 1, outfile);
			fwrite((char*)&fileio[0].weight, sizeof(ULL), 1, outfile);
			fclose(outfile);
		}
		else {
			node_number = char_kind * 2 - 1;
			tree.resize(node_number + 1);
			i = 0;
			for (i; i < char_kind; ++i) {
				tree[i].weight = fileio[i].weight;
				tree[i].character = fileio[i].character;
				tree[i].parent = 0;
			}
			for (i; i < node_number; ++i) {
				tree[i].parent = 0;
			}
			CreateTree();
			CreateCode();
			fwrite((char*)&char_kind, sizeof(size_t), 1, outfile);
			for (i = 0; i < char_kind; ++i) {
				fwrite((char*)&tree[i].character, sizeof(bit8), 1, outfile);
				fwrite((char*)&tree[i].weight, sizeof(ULL), 1, outfile);
			}
			fwrite((char*)&file_len, sizeof(ULL), 1, outfile);
			fopen_s(&infile, ifile_name.c_str(), "rb");
			ULL write_len = 0;
			do {
				fread((char*)&inchar, sizeof(bit8), 1, infile);
				if (write_len==file_len) break;
				fputs(code[inchar].c_str(), outfile);
				++write_len;
			} while (true);
			fclose(infile);
			fclose(outfile);
		}
		return 0;
	}
	int hfm::HFMZip::CreateFile(const std::string& ifile_name,
		const std::string& ofile_name) {
		size_t i = 0;
		ULL write_len = 1;
		ULL file_len = 0;
		bit8 inchar = 0;
		char file_kind[20];
		int now;
		fopen_s(&infile, ifile_name.c_str(), "rb");
		if (infile == nullptr) return -1;
		for (i = 0; i < 20; ++i) {
			inchar = fgetc(infile);
			if (inchar == 1)break;
			else file_kind[i] = inchar;
		}
		fread((char*)&char_kind, sizeof(size_t), 1, infile);
		if (char_kind == 1){
			fread((char*)&inchar, sizeof(bit8), 1, infile);
			fread((char*)&file_len, sizeof(ULL), 1, infile);
			fopen_s(&outfile, ofile_name.c_str(), "wb");
			if (outfile == nullptr) return -2;
			while (file_len--) {
				fwrite((char*)&inchar, sizeof(unsigned char), 1, outfile);
			}
			fclose(infile);
			fclose(outfile);
		}
		else {
			node_number = char_kind * 2 - 1;
			tree.resize(node_number + 1);
			for (i = 0; i < char_kind; ++i) {
				fread((char*)&tree[i].character, sizeof(bit8), 1, infile);
				fread((char*)&tree[i].weight, sizeof(ULL), 1, infile);
				tree[i].parent = 0;
			}
			for (i; i < node_number; ++i) {
				tree[i].parent = 0;
			}
			CreateTree();
			fread((char*)&file_len, sizeof(ULL), 1, infile);
			fopen_s(&outfile, ofile_name.c_str(), "wb");
			now = node_number - 1;
			while (true) {
				fread((char*)&inchar, sizeof(bit8), 1, infile);
				if (inchar == '1') {
					now = tree[now].right_child;
				}
				else if (inchar == '0') {
					now = tree[now].left_child;
				}
				else return -1;
				if (write_len == file_len)break;
				if (tree[now].left_child == 0 && tree[now].right_child == 0) {
					fwrite((char*)&tree[now].character, sizeof(bit8), 1, outfile);
					now = node_number - 1;
					++write_len;
				}
			}
			fclose(infile);
			fclose(outfile);
		}
		return 0;
	}
	int hfm::compress(const std::string& ifile_name) {
		std::string ofile_name = ifile_name;
		for (size_t i = ofile_name.size() - 1; i > 0; --i) {
			if (ofile_name[i] == '.') {
				ofile_name.erase(i, ofile_name.size() - i);
				break;
			}
		}
		ofile_name += ".huf";
		return compress(ifile_name, ofile_name);
	}
	int hfm::compress(const std::string& ifile_name,
		const std::string& ofile_name) {
		HFMZip opreation;
		opreation.LoadFile(ifile_name, ofile_name);
		return 0;
	}
	bool hfm::StatisticCMP(const Statistic& a, const Statistic& b)
	{
		return a.weight > b.weight;
	}
	int hfm::extract(const std::string& ifile_name) {
		FILE* infile;
		char filekind[20] = "";
		bit8 inchar;
		std::string ofile_name = ifile_name;
		size_t i = ofile_name.size() - 1;
		while (ofile_name[i] != '.' && i > 0)--i;
		if (i <= 0)return -1;
		ofile_name.erase(i);
		fopen_s(&infile, ifile_name.c_str(), "rb");
		for (i = 0; i < 20; ++i) {
			inchar = fgetc(infile);
			if (inchar == 1)break;
			else filekind[i] = inchar;
		}
		ofile_name += filekind;
		return extract(ifile_name, ofile_name);
	}
	int hfm::extract(const std::string& ifile_name,
		const std::string& ofile_name) {
		HFMZip opreation;
		opreation.CreateFile(ifile_name, ofile_name);
		return 0;
	}
