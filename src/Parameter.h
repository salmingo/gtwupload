
#ifndef PARAMETER_H_
#define PARAMETER_H_

#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

struct Parameter {
public:
	std::string hostName;	/// FTP服务器IP地址或DNS名称
	int hostPort;			/// FTP服务器端口
	std::string account;	/// 账号
	std::string dirLocal;	/// 本地主机上的目录
	std::string dirRemote;	/// 远程主机上的目录

public:
	/*!
	 * @brief 初始化配置参数文件
	 * @param filepath 文件路径
	 */
	void Init(const std::string &filepath) {
		using namespace boost::property_tree;
		using namespace boost::property_tree::xml_parser;
		using namespace boost::posix_time;

		ptree nodes;
		nodes.add("version", "0.1");
		nodes.add("date", to_iso_extended_string(second_clock::universal_time()));

		ptree& node1 = nodes.add("FtpServer", "");
		node1.add("<xmlattr>.Name", "");
		node1.add("<xmlattr>.Port", 20);

		ptree& node2 = nodes.add("Account", "");
		node2.add("<xmlattr>.Name", "");

		ptree& node3 = nodes.add("Data", "");
		node3.add("Local.<xmlattr>.Directory", "");
		node3.add("Remote.<xmlattr>.Directory", "");

		xml_writer_settings<std::string> settings(' ', 4);
		write_xml(filepath, nodes, std::locale(), settings);
	}

	/*!
	 * @brief 加载配置参数文件
	 * @param filepath 文件路径
	 * @return
	 * 文件加载结果
	 */
	bool Load(const std::string &filepath) {
		using namespace boost::property_tree;
		using namespace boost::property_tree::xml_parser;

		try {
			std::string str;
			ptree nodes;

			read_xml(filepath, nodes, trim_whitespace);
			BOOST_FOREACH(ptree::value_type const &child, nodes.get_child("")) {
				if (boost::iequals(child.first, "FtpServer")) {
					hostName = child.second.get("<xmlattr>.Name",    "");
					hostPort = child.second.get("<xmlattr>.Port",    20);
				}
				else if (boost::iequals(child.first, "Account")) {
					account = child.second.get("<xmlattr>.Name",    "");
				}
				else if (boost::iequals(child.first, "Data")) {
					dirLocal  = child.second.get("Local.<xmlattr>.Directory",    "");
					dirRemote = child.second.get("Remote.<xmlattr>.Directory",   "");
				}
			}

			return true;
		}
		catch(xml_parser_error &ex) {
			return false;
		}
	}
};

#endif
