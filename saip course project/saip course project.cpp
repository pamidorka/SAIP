
#ifndef linux
#pragma warning(disable : 4996)
#endif

#include <iostream>

#include <string.h>

struct Note {
	char full_name[32];
	char street[18];
	short int house_number;
	short int apartment_number;
	char date[10];
};

struct List {
	Note* data;
	List* next;
};

struct Vertex {
	Note* data;
	Vertex* left;
	Vertex* right;
	bool balance;
};

struct QBody {
	List* head;
	List* tail;
};

List* CreateListFromDataBase(FILE* base, unsigned int base_size) {
	List* head = new List;
	List* temp = head;

	for (unsigned int i = 0; i < base_size - 1; i++) {
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

void PrintNote(Note note, std::uint32_t idx) {
	std::cout << idx + 1 << ") "
		<< note.full_name << "\t"
		<< note.street << "\t"
		<< note.house_number << "\t"
		<< note.apartment_number << "\t"
		<< note.date << std::endl;
}

void PrintList(List* head) {
	if (!head) {
		return;
	}

#ifdef linux
	system("clear");
#else
	system("cls");
#endif
	List* temp = head;
	int number_of_note = 0;

	do {
		PrintNote(*(temp->data), number_of_note);
		temp = temp->next;
		number_of_note++;
	} while (temp);

}

void DigitalSort(List*& head) {
	List* point;
	QBody queues[256];
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
				queue_num = head->data->street[(street_bytes - 1) - (j - house_num_bytes)] + 128;
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

List** CreateIndexArray(List* root, unsigned int size) {
	List** arr = new List* [size];
	List* tmp = root;

	for (unsigned int i = 0; (i < size) && tmp; i++) {
		arr[i] = tmp;
		tmp = tmp->next;
	}

	return arr;
}

List* BinarySearch(List** index_arr, char element[18], unsigned int size) {
	int left = 0;
	int right = size - 1;

	while (left < right) {
		int m = (left + right) / 2;
		if (strncmp(index_arr[m]->data->street, element, 3) < 0) left = m + 1; 
		else right = m;
	}

	if (!strncmp(index_arr[right]->data->street, element, 3)) {
		List* root = new List;
		List* temp = root;
		temp->data = new Note;
		(*temp->data) = (*index_arr[right]->data);
		for (unsigned int i = right + 1; i < size && index_arr[i]->data && !strncmp(index_arr[i]->data->street, element, 3); i++) {
			temp->next = new List;
			temp = temp->next;
			temp->data = new Note;
			*(temp->data) = *(index_arr[i]->data);
			temp->next = NULL;
		}
		return root;
	}
	std::cout << "Not found" << std::endl;
	return nullptr;
}

void AddBBT(Note* data, Vertex*& point) {
	static bool vr = true;
	static bool hr = true;
	if (!point) {
		point = new Vertex;
		point->data = data;
		point->left = nullptr;
		point->right = nullptr;
		point->balance = false;
		vr = true;
	}
	else if (data->apartment_number < point->data->apartment_number) {
		AddBBT(data, point->left);
		if (vr == true) {
			if (!point->balance) {
				Vertex* q = point->left;
				point->left = q->right;
				q->right = point;
				point = q;
				q->balance = true;
				vr = false;
				hr = true;
			}
			else {
				point->balance = false;
				vr = true;
				hr = false;
			}
		}
		else {
			hr = false;
		}
	}
	else if (data->apartment_number >= point->data->apartment_number) {
		AddBBT(data, point->right);
		if (vr == true) {
			point->balance = true;
			hr = true;
			vr = false;
		}
		else if (hr == true) {
			if (point->balance) {
				Vertex* q = point->right;
				point->balance = false;
				q->balance = false;
				point->right = q->left;
				q->left = point;
				point = q;
				vr = true;
				hr = false;
			}
			else {
				hr = false;
			}
		}
	}
}

Vertex* CreateBBT(List* arr) {
	Vertex* root = nullptr;
	for (List* i = arr; i; i = i->next) {
		AddBBT(i->data, root);
	}
	return root;
}

void PrintBBD(const Vertex* root, std::uint32_t number_of_note) {
	if (root) {
		PrintBBD(root->left, number_of_note);
		PrintNote(*(root->data), number_of_note++);
		PrintBBD(root->right, number_of_note);
	}
}

void PrintIndexArr(List** arr, std::uint32_t size) {
	for (std::uint32_t i = 0; i < size; i++) {
		PrintNote(*(arr[i]->data), i);
	}
}

void MemoryCleaner(List* main_data_base, List** unsorted_index_array_list, List** sorted_index_array_list) {

	List* tmp;
	for (List* i = main_data_base; i;) {
		tmp = i->next;
		delete i;
		i = tmp;
	}

	delete[] unsorted_index_array_list;
	delete[] sorted_index_array_list;
}

void RemoveBBT(Vertex* root) {
	if (root) {
		RemoveBBT(root->left);
		RemoveBBT(root->right);
		delete root;
	}
}

int GetOption(char street_name[18]) {
	int opt;
	std::cout << "Select option" << std::endl
		<< "1 - Print unsorted data base" << std::endl
		<< "2 - Print sorted data base (Digital sort by street name and house number)" << std::endl
		<< "3 - Print binary B-Tree by apartment number from Binary search by street name" << std::endl
		<< "4 - Binary search by street name" << std::endl
		<< "5 - Print coding (not implemented)" << std::endl
		<< "Other key is exit from the program" << std::endl;
	opt = getchar();
#ifdef linux
	system("clear");
#else
	system("cls");
#endif

	if (opt == '3' || opt == '4') {
		std::cout << "Enter the street name you are looking for" << std::endl;
		std::cin >> street_name;
	}

	return opt;
}

void MainMenu(List** unsorted_index_array_list, List** sorted_index_array_list, std::uint32_t base_size) {
		
	char street_name[18];
	List* elements = nullptr;
	Vertex* tree_root = nullptr;

	switch (GetOption(street_name)) {
	case '1':
		PrintIndexArr(unsorted_index_array_list, base_size);
		break;
	case '2':
		PrintIndexArr(sorted_index_array_list, base_size);
		break;
	case '3':
		if (!elements) {
			elements = BinarySearch(sorted_index_array_list, street_name, base_size);
		}
		tree_root = CreateBBT(elements);
		PrintBBD(tree_root, 0);
		break;
	case '4':
		elements = BinarySearch(sorted_index_array_list, street_name, base_size);
		PrintList(elements);
		break;
	case '5':
		std::cout << "Function not implemented!" << std::endl;
		break;
	default:
		std::cout << "Program closed" << std::endl;
		break;
	}

	RemoveBBT(tree_root);

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

	fclose(base);

	List** unsorted_index_array_list = CreateIndexArray(head, kData_base_size);

	DigitalSort(head);
	
	List** sorted_index_array_list = CreateIndexArray(head, kData_base_size);

	MainMenu(unsorted_index_array_list, sorted_index_array_list, kData_base_size);

	MemoryCleaner(head, unsorted_index_array_list, sorted_index_array_list);
	exit(EXIT_SUCCESS);
}
