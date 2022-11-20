
/*



	WARNING!!!

	WHILE YOU DON'T LOOKED THE CODE, PLEASE READ THIS WARNING

	This code was written for the university;
	I did not set goals to write a competent and understandable code;
	If you want to look at more literal code please look at my other repositories;

	But if you want to look at my knowledge of algorithms then keep watching this code :)



*/




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

struct Body {
	List* head;
	List* tail;
};

void DigitalSort(List*& head) {
	List* point;
	Body queues[256];
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
		if (strncmp(index_arr[m]->data->street, element, 3) < 0) left = m + 1; // segmentation fault in the linux; 3 notes is lost in index arr; fixed.;
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
	return nullptr;
}

struct Vertex {
	Note* data;
	Vertex* left;
	Vertex* right;
	int balance;
};

void AddDBD(Note* data, Vertex*& point) {
	static int vr = 1;
	static int hr = 1;
	if (!point) {
		point = new Vertex;
		point->data = data;
		point->left = nullptr;
		point->right = nullptr;
		point->balance = 0;
		vr = 1;
	}
	else if (data->apartment_number < point->data->apartment_number) {
		AddDBD(data, point->left);
		if (vr == 1) {
			if (point->balance == 0) {
				Vertex* q = point->left;
				point->left = q->right;
				q->right = point;
				point = q;
				q->balance = 1;
				vr = 0;
				hr = 1;
			}
			else {
				point->balance = 0;
				vr = 1;
				hr = 0;
			}
		}
		else {
			hr = 0;
		}
	}
	else if (data->apartment_number >= point->data->apartment_number) {
		AddDBD(data, point->right);
		if (vr == 1) {
			point->balance = 1;
			hr = 1;
			vr = 0;
		}
		else if (hr == 1) {
			if (point->balance == 1) {
				Vertex* q = point->right;
				point->balance = 0;
				q->balance = 0;
				point->right = q->left;
				q->left = point;
				point = q;
				vr = 1;
				hr = 0;
			}
			else {
				hr = 0;
			}
		}
	}
}

Vertex* CreateDBD(List* arr) {
	Vertex* root = nullptr;
	for (List* i = arr; i; i = i->next) {
		AddDBD(i->data, root);
	}
	return root;
}

void PrintDBD(const Vertex* root, std::uint32_t number_of_note) {
	if (root) {
		PrintDBD(root->left, number_of_note);
		PrintNote(*(root->data), number_of_note++);
		PrintDBD(root->right, number_of_note);
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

void RemoveDBD(Vertex* root) {
	if (root) {
		RemoveDBD(root->left);
		RemoveDBD(root->right);
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
		tree_root = CreateDBD(elements);
		PrintDBD(tree_root, 0);
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

	RemoveDBD(tree_root);

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
