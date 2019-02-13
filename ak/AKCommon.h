//
// Created by Sunny Yang on 2019-02-12.
//

#ifndef AKSERVER_AKCOMMON_H
#define AKSERVER_AKCOMMON_H

namespace ak
{
	//公用类
	class AKCommon
	{
	public:
		AKCommon();
		~AKCommon();

		static int32 argc;
		static int8 **argv;
		
		//生成uuid
		static const std::string uuid();

		//crc 32位求和校验
		static int32 crc32(const int8 *data,uint32 bytes);

		//sha1编码
		static std::string sha1(const int8 *data,uint32 bytes);

		//base64编码
		static std::string base64(
			const std::string &input//字符串
			);

		//base64解码
		static std::string unbase64(
			const std::string &input
			);

		//脚本URL解释//URL开头如果存在'?'，则自动替换为script所在目录
		static std::string scriptUrlFormat(
			AKScript *script,//脚本对象
			const int8 *scriptUrl//脚本URL
			);

		//查找指定目录的文件并保存到文件字典
		static void fileList(
			container::AKDict &filePathDict,//保存搜索结果的文件字典
			boost::filesystem::path &findDir//查找目录
			);

		//格式化路径（把'\\'转为'/'把"."目录和".."目录合并消除）
		//例如：\a\..b\将格式化为/b/
		static std::string dirFormat(
			std::string path//路径
			);

		//字符串替换
		static void strReplace(
			std::string &srcStr,//源字符串
			std::string findStr,//查找的字符串
			std::string replaceStr//替换的字符串
			);

		//C风格字符串转义
		static void str_c_escape(
			std::string &srcStr
			);

		//C风格字符串逆转义
		static void str_c_unescape(
			std::string &srcStr
			);

		//创建目录
		static bool createDir(
			const int8 *dir//目录路径
			);

		//休眠
		static void sleep(
			uint32 dt//时间（毫秒）
			);

		//md5
		static std::string MD5(
			container::AKBuf &buf//数据缓冲
			);

		//md5
		static std::string MD5(
			const int8 *data,//数据
			uint32 len//尺寸
			);

		//是否为无符号整数
		static bool isUint(const int8 *str);

		//域名转ip
		static std::string domain2IP(std::string domain);

		//转换为uint索引
		static bool string2UintIndex(
			const int8 *index,
			uint32 &uintIndex
			);

		//求两点间距离
		static double vectLength(
			double x1,double y1,//点1
			double x2,double y2//点2
			);

		//求点到线的交点
		static void pointCrossLine(
			double &xc,double &yc,//交点
			double x1,double y1,//直线上点1
			double x2,double y2,//直线上点2
			double x3,double y3//垂线上的点
			);

		//角度转弧度
		static double angle2radian(double angle);

		//弧度转角度
		static double radian2angle(double radian);

		//获取向量角度
		static double vectAngle(double x0,double y0,double x1,double y1);

		//计算直线移动的位置
		static void moveTo(
		double &x,//开始位置（计算完成后保存结果的变量）
		double &y,//开始位置（计算完成后保存结果的变量）
		double xDest,//目标位置
		double yDest,//目标位置
		double speed,//移动速度
		double dt//经过的时间（毫秒）
		);

		//获取日期时间信息
		static void timeInfo(
			uint32 &yy,//年
			uint32 &mm,//月
			uint32 &dd,//日
			uint32 &h,//时
			uint32 &m,//分
			uint32 &s//秒
			);

		//缓冲转十六进制字符串
		static void buf2hex(
			std::string &hexStr,//目标字符串
			container::AKBuf *buf//数据缓冲
			);

		//十六进制字符串转缓冲
		static bool hex2buf(
			container::AKBuf *buf,//数据缓冲
			std::string &hexStr//十六进制字符串
			);

		//返回日期时间字符串
		static std::string now();

		//序列化//失败返回false和error
		static bool serialize(
			AKCore *core,//核对象
			container::AKBuf *rtBuf,//接收序列化数据的缓冲
			AKVar *var,//需要序列化的变量
			std::string &error//错误信息
			);

		//反序列化
		static bool unserialize(
			AKCore *core,//核对象
			AKVar *var,//接收结果的变量
			container::AKBuf *buf//序列化数据的缓冲
			);

		//克隆
		static bool clone(
			AKCore *core,//核对象
			AKVar *newVar,//接收对象
			AKVar *var,//被克隆的对象
			std::string &error//错误信息
			);

		//设置容器变量值
		static bool setVarToContainer(
			AKScript *script,
			AKOrder *order,
			int8 *error
			);

		//从容器中获取变量
		static AKVar *getVarFromContainer(
			AKScript *script,
			AKOrder *order,
			int8 *error,
			bool isAutoCreateVar=false//变量不存在时，自动创建未初始化的变量
			);

		//输出日志
		static void log(
			const int8 *filePath,
			const int8 *logStr
			);

		//输出日志
		static void log(
			const int8 *filePath,
			std::string &logStr
			);

		//返回时间整数（秒）
		static double time();

		//返回时间整数（毫秒）
		static double time_milli();

		//返回时间整数（微秒）
		static double time_micro();

		//返回时间整数（纳秒）
		static double time_nano();

		//var转json字符串
		static void json_encode(
			AKCore *core,//核对象
			AKVar *var,//变量
			std::string &outStr//接受输出的字符串
			);

		//url编码
		static std::string urlencode(const std::string &str);

		//url解码
		static std::string urldecode(const std::string &str);

		//字符串分割
		static void split(
			AKStringList &strList,//分割后保存的数组
			const int8 *str,//源字符串
			const int8 *spliter//分割字符串
			);

		//时间（从1970年开始计算的秒数）转日期时间字符串
		static std::string timetostr(time_t time);

	};
}

#endif //AKSERVER_AKCOMMON_H
