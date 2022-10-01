#pragma warning(disable : 4996)

#include <iostream>

struct Note {
	char full_name[32];
	char street[18];
	short int house_number;
	short int apartament_number;
	char date[10];
};

struct List {
	Note* data;
	List* next;
};

List* CreateListFromDataBase(FILE* base, int base_size) {
	List* head = new List;
	List* temp = head;

	for (int i = 0; i < base_size - 1; i++) {
		temp->data = new Note;
		fread((Note*)temp->data, sizeof(Note), 1, base);

		temp->next = new List;
		temp = temp->next;
	}

	temp->data = new Note;
	fread((Note*)temp->data, sizeof(Note), 1, base);
	temp->next = NULL;

	return head;
}

void PrintDataBaseFromList(List* head) {
	bool output_data = true;
	List* temp = head;
	int number_of_note = 0;

	while (output_data) {
		for (int i = 0; i < 20; i++, number_of_note++) {
			std::cout << number_of_note + 1 << ") " 
				<< temp->data->full_name << "\t" 
				<< temp->data->street << "\t" 
				<< temp->data->house_number << "\t" 
				<< temp->data->apartament_number << "\t" 
				<< temp->data->date << std::endl;

			if (temp->next == NULL) {
				return;
			}

			temp = temp->next;
		}

		std::cout << "print next page? 1 or 0" << std::endl;
		std::cin >> output_data;
		system("cls");
	}

}

static struct Body {
	List* head;
	List* tail;
};

void DigitalSort(List*& head) {
	List* point;
	Body queues[256];
	char* house_num_ptr;
	unsigned int street_bytes = 3;
	unsigned int house_num_bytes = 2;
	unsigned int length = street_bytes + house_num_bytes;

	for (unsigned int j = 0; j < length; j++) {
		for (auto& i : queues) {
			i.tail = i.head = nullptr;
		}

		while (head) {
			unsigned int queue_num;
			if (j < house_num_bytes) {
				queue_num = *((char*)&(head->data->house_number) + j);
			}
			else if (j < length) {
				queue_num = 255 - abs(head->data->street[j - house_num_bytes]);
			}

			point = queues[queue_num].tail;

			if (queues[queue_num].head == nullptr) {
				queues[queue_num].head = head;
			}
			else {
				point->next = head;
			}

			point = queues[queue_num].tail = head;
			head = head->next;
			point->next = nullptr;
		}

		int i;
		for (i = 0; i < 256; i++) {
			if (queues[i].head != nullptr) {
				break;
			}
		}

		head = queues[i].head;
		point = queues[i].tail;
		for (unsigned int k = i + 1; k < 256; k++) {
			if (queues[k].head != nullptr) {
				point->next = queues[k].head;
				point = queues[k].tail;
			}
		}
	}
}

int main() {
	FILE* base;
	const unsigned int kData_base_size = 4000;

	base = fopen("testBase4.dat", "rb");
	if (!base) {
		std::cout << "Data base dont open, abort" << std::endl;
		exit(EXIT_FAILURE);
	}

	List* head = CreateListFromDataBase(base, kData_base_size);

	DigitalSort(head);
	
	PrintDataBaseFromList(head);

	exit(EXIT_SUCCESS);
}
