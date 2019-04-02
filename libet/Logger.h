#pragma once

#include "Util.h"
#include <iostream>
//#include <QDebug>

#ifdef WIN32  
#define errcode WSAGetLastError()
#endif

#ifdef _WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '\\') + 1):__FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)
#endif

//����һ������־����� �ļ��� ������ �к� �ĺ궨��
#ifndef suffix
#define suffix(msg)  std::string(msg).append("  <")\
        .append(__FILENAME__).append("> <").append(__func__)\
        .append("> <").append(std::to_string(__LINE__))\
        .append(">").c_str()
#endif

//��spdlog.h֮ǰ�������Ч
#ifndef SPDLOG_TRACE_ON
#define SPDLOG_TRACE_ON
#endif

#ifndef SPDLOG_DEBUG_ON
#define SPDLOG_DEBUG_ON
#endif

#include <spdlog\spdlog.h>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <spdlog\sinks\daily_file_sink.h>

static bool g_spdlogDestructed = false;

class Logger
{
public:
	static Logger& GetInstance() {
		static Logger m_instance;
		return m_instance;
	}

	auto GetLogger() { return m_logger; }

private:
	Logger() {
		util::MakeSureDirExist("logs");

		//����Ϊ�첽��־
		//spdlog::set_async_mode(32768);  // ����Ϊ 2 ����
		std::vector<spdlog::sink_ptr> sinkList;
#ifdef _CONSOLE
		auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		consoleSink->set_level(spdlog::level::debug);
		//consoleSink->set_pattern("[multi_sink_example] [%^%l%$] %v");
		consoleSink->set_pattern("[%m-%d %H:%M:%S.%e][%^%L%$]  %v");
		sinkList.push_back(consoleSink);
#endif
		auto basicSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/basicSink.txt");
		basicSink->set_level(spdlog::level::debug);
		basicSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%5l%$]  %v");
		sinkList.push_back(basicSink);
		m_logger = std::make_shared<spdlog::logger>("both", begin(sinkList), end(sinkList));

		//register it if you need to access it globally
		spdlog::register_logger(m_logger);

		// ������־��¼����
#ifdef _DEBUG
		m_logger->set_level(spdlog::level::trace);
		//m_logger->set_level(spdlog::level::debug);
#else
		m_logger->set_level(spdlog::level::err);
#endif
		//���õ�����err������صĴ���ʱ����ˢ����־��disk 
		m_logger->flush_on(spdlog::level::err);
		spdlog::flush_every(std::chrono::seconds(3));

		//m_logger->set_error_handler([](const std::string& msg) {
		//	throw std::runtime_error(msg);
		//});
	}

	~Logger() {
		g_spdlogDestructed = true;
		//qDebug() << "Logger dtor";
		//���ܵ��ã���������
		//spdlog::drop_all();

	}
	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;

private:
	std::shared_ptr<spdlog::logger> m_logger;
};

//#define SPDLOGGER Logger::GetInstance().GetLogger()
//#define LTrace(msg,...) \
//	do { \
//		if(!g_spdlogDestructed) { \
//			SPDLOGGER->trace(suffix(msg),__VA_ARGS__); \
//		} \
//	} while (0)
//#define LDebug(...) \
//	do { \
//		if(!g_spdlogDestructed) { \
//			SPDLOGGER->debug(__VA_ARGS__); \
//		} \
//	} while (0)
//#define LInfo(...) \
//	do { \
//		if(!g_spdlogDestructed) { \
//			SPDLOGGER->info(__VA_ARGS__); \
//		} \
//	} while (0)
//#define LWarn(...) \
//	do { \
//		if(!g_spdlogDestructed) { \
//			SPDLOGGER->warn(__VA_ARGS__); \
//		} \
//	} while (0)
//#define LError(...) \
//	do { \
//		if(!g_spdlogDestructed) { \
//			SPDLOGGER->error(__VA_ARGS__); \
//		} \
//	} while (0)
//#define LCritical(...) \
//	do { \
//		if(!g_spdlogDestructed) { \
//			SPDLOGGER->critical(__VA_ARGS__); \
//		} \
//	} while (0)
//#define criticalif(b, ...)                        \
//    do {                                       \
//		if(!g_spdlogDestructed) { \
//			if ((b)) {                             \
//				SPDLOGGER->critical(__VA_ARGS__); \
//			}                                      \
//		}											\
//	} while (0)


//#define LTrace(msg,...) Logger::GetInstance().GetLogger()->trace(suffix(msg),__VA_ARGS__)
//#define LDebug(...) Logger::GetInstance().GetLogger()->debug(__VA_ARGS__)
//#define LInfo(...) Logger::GetInstance().GetLogger()->info(__VA_ARGS__)
//#define LWarn(...) Logger::GetInstance().GetLogger()->warn(__VA_ARGS__)
//#define LError(...) Logger::GetInstance().GetLogger()->error(__VA_ARGS__)
//#define LCritical(...) Logger::GetInstance().GetLogger()->critical(__VA_ARGS__)
//
//#define criticalif(b, ...)                        \
//    do {                                       \
//        if ((b)) {                             \
//           Logger::GetInstance().GetLogger()->critical(__VA_ARGS__); \
//        }                                      \
//    } while (0)


//FIXME: spdlog��־����ڱ������������ε�
//�о�����־�������ˣ���־���ڴ�ӡ
//spdlog::logger::name_�������������쳣: ��ȡ����Ȩ�޳�ͻ��this->�� 0xDDDDDDDD��
#define LTrace(msg,...)
#define LDebug(...) 
#define LInfo(...) 
#define LWarn(...) 
#define LError(...) 
#define LCritical(...) 

#define criticalif(b, ...)
