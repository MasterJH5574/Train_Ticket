#include "users.hpp"
#include "TrainManager.hpp"
#include <fstream>
#include <iostream>

void read_command(std::istream& is, std::ostream& os) {
	users user_manager;
	TrainManager  train_Manage;
	int f = 0;
	char command[50];
	while (is >> command) {
		if (strcmp(command, "register") == 0) {
			os << user_manager.user_register(is, os);
			continue;
		}
		if (strcmp(command, "login") == 0) {
			os << user_manager.user_login(is, os);
			continue;
		}
		if (strcmp(command, "query_profile") == 0) {
			user_manager.query_user(is, os);
			continue;
		}
        if (strcmp(command, "query_password") == 0) {
			user_manager.query_password(is, os);
			continue;
		}
		if (strcmp(command, "modify_profile") == 0) {
			os << user_manager.modify_user(is, os);
			continue;
		}
		if (strcmp(command, "modify_privilege") == 0) {
			os << user_manager.modify_user_priv(is, os);
			continue;
		}
		if (strcmp(command, "add_train") == 0) {
			os << train_Manage.add_train(is, os);
			continue;
		}
		if (strcmp(command, "sale_train") == 0) {
			os << train_Manage.sale_train(is, os);
			continue;
		}
		if (strcmp(command, "delete_train") == 0) {
			os << train_Manage.delete_train(is, os);
			continue;
		}
		if (strcmp(command, "query_train") == 0) {
			train_Manage.query_train(is, os);
			continue;
		}
		if (strcmp(command, "modify_train") == 0) {
			os << train_Manage.modify_train(is, os);
			continue;
		}

		if (strcmp(command, "query_ticket") == 0) {
            train_Manage.query_ticket(is, os);
            continue;
		}

        if (strcmp(command, "buy_ticket") == 0) {
            os << train_Manage.buy_ticket(is, os);
            continue;
        }

        if (strcmp(command, "refund_ticket") == 0) {
            os << train_Manage.refund_ticket(is, os);
            continue;
        }

        if (strcmp(command, "query_order") == 0) {
            train_Manage.query_order(is, os);
            continue;
        }
        if (strcmp(command, "query_order_all") == 0) {
            train_Manage.query_order_alldate(is, os);
            continue;
        }
        if (strcmp(command, "query_transfer") == 0) {
            train_Manage.query_transfer(is, os);
            continue;
        }
        if (strcmp(command, "exist_train") == 0) {
            os << train_Manage.exist_train(is, os);
            continue;
        }



		if (strcmp(command, "clean") == 0) {
			os << "1 ";
			continue;
		}
		if (strcmp(command, "exit") == 0) {
            os << "BYE ";
            break;
        }
	}
}
