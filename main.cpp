#include <QtGui/QApplication>
#include "htv_mainwindow.h"
#include "htv_replicationmanager.h"
#include "core/htv_config.h"


int main(int argc, char *argv[])
{
   QApplication a(argc, argv);
   C_MainWindow w;
   w.show();
	/*C_ReplicationManagerConfig *manager = C_ReplicationManagerConfig::getInstance();

	C_Host* localhost = manager->M_GetLocalHost();*/
	/*C_ReplicationConnection *connection = new C_ReplicationConnection();
	connection->connectToHost(QHostAddress::LocalHost, 1704);
*/
	C_ReplicationManager manager;

   return a.exec();
}
