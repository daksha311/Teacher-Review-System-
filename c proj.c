#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_REVIEW_LENGTH 256
#define MAX_TEACHER_NAME 50
#define MAX_LINE_LENGTH 512

// Write review and rating into department file
void writeReviewToFile(const char *department, const char *teacherName, const char *review, int rating) {
	FILE *file;
	char filename[50];

	snprintf(filename, sizeof(filename), "%s_reviews.txt", department);
	file = fopen(filename, "a");
	if (!file) {
		printf("Error opening file for writing.\n");
		return;
	}

	fprintf(file, "Teacher: %s\nReview: %s\nRating: %d\n\n", teacherName, review, rating);
	fclose(file);

	printf("Review saved successfully for teacher %s in %s department.\n", teacherName, department);
}

// Display all reviews for a department
void displayReviews(const char *department) {
	FILE *file;
	char filename[50], line[MAX_LINE_LENGTH];

	snprintf(filename, sizeof(filename), "%s_reviews.txt", department);
	file = fopen(filename, "r");
	if (!file) {
		printf("No reviews available for the %s department.\n", department);
		return;
	}

	printf("\nReviews for %s department:\n", department);
	while (fgets(line, sizeof(line), file)) {
		printf("%s", line);
	}

	fclose(file);
}

// Search and display reviews for a specific teacher in a department
void searchReviewsByTeacher(const char *department, const char *teacherName) {
	FILE *file;
	char filename[50], line[MAX_LINE_LENGTH];
	int found = 0;

	snprintf(filename, sizeof(filename), "%s_reviews.txt", department);
	file = fopen(filename, "r");
	if (!file) {
		printf("No reviews available for the %s department.\n", department);
		return;
	}

	printf("\nReviews for teacher \"%s\" in %s department:\n", teacherName, department);

	while (fgets(line, sizeof(line), file)) {
		if (strncmp(line, "Teacher: ", 9) == 0) {
			if (strstr(line + 9, teacherName) != NULL) {
				found = 1;
				printf("%s", line);
				if (fgets(line, sizeof(line), file)) {
					printf("%s", line);
				}
				if (fgets(line, sizeof(line), file)) {
					printf("%s", line);
				}
				printf("\n");
			}
		}
	}

	fclose(file);
	if (!found) {
		printf("No reviews found for teacher %s in %s department.\n", teacherName, department);
	}
}

// Edit review and rating for a teacher
void editReview(const char *department, const char *teacherName, const char *newReview, int newRating) {
	FILE *file, *tempFile;
	char filename[50], tempFilename[] = "temp_reviews.txt";
	char line[MAX_LINE_LENGTH];
	int found = 0;

	snprintf(filename, sizeof(filename), "%s_reviews.txt", department);

	file = fopen(filename, "r");
	if (!file) {
		printf("Error opening file for editing.\n");
		return;
	}
	tempFile = fopen(tempFilename, "w");
	if (!tempFile) {
		printf("Error opening temporary file for writing.\n");
		fclose(file);
		return;
	}

	while (fgets(line, sizeof(line), file)) {
		if (!found && strncmp(line, "Teacher: ", 9) == 0 && strstr(line + 9, teacherName) != NULL) {
			fprintf(tempFile, "Teacher: %s\nReview: %s\nRating: %d\n\n", teacherName, newReview, newRating);
			fgets(line, sizeof(line), file);
			fgets(line, sizeof(line), file);
			found = 1;
		} else {
			fputs(line, tempFile);
		}
	}

	fclose(file);
	fclose(tempFile);

	if (found) {
		remove(filename);
		rename(tempFilename, filename);
		printf("Review for %s edited successfully in %s department.\n", teacherName, department);
	} else {
		remove(tempFilename);
		printf("Teacher not found in %s department.\n", department);
	}
}

// Delete all reviews of a teacher in department
void deleteReview(const char *department, const char *teacherName) {
	FILE *file, *tempFile;
	char filename[50], tempFilename[] = "temp_reviews.txt";
	char line[MAX_LINE_LENGTH];
	int found = 0;

	snprintf(filename, sizeof(filename), "%s_reviews.txt", department);

	file = fopen(filename, "r");
	if (!file) {
		printf("Error opening file for reading.\n");
		return;
	}
	tempFile = fopen(tempFilename, "w");
	if (!tempFile) {
		printf("Error opening temporary file for writing.\n");
		fclose(file);
		return;
	}

	while (fgets(line, sizeof(line), file)) {
		if (strncmp(line, "Teacher: ", 9) == 0 && strstr(line + 9, teacherName) != NULL) {
			fgets(line, sizeof(line), file);
			fgets(line, sizeof(line), file);
			fgets(line, sizeof(line), file);
			found = 1;
		} else {
			fputs(line, tempFile);
		}
	}

	fclose(file);
	fclose(tempFile);

	if (found) {
		remove(filename);
		rename(tempFilename, filename);
		printf("All reviews for %s deleted successfully in %s department.\n", teacherName, department);
	} else {
		remove(tempFilename);
		printf("Teacher not found in %s department.\n", department);
	}
}

// Validate non-empty review
int validateReview(const char *review) {
	if (strlen(review) == 0) {
		printf("Review cannot be empty.\n");
		return 0;
	}
	return 1;
}

// Validate non-empty teacher name
int validateTeacherName(const char *name) {
	if (strlen(name) == 0) {
		printf("Teacher name cannot be empty.\n");
		return 0;
	}
	return 1;
}

// Calculate and display average rating for each teacher
void calculateAverageRatings(const char *department) {
	FILE *file;
	char filename[50], line[MAX_LINE_LENGTH];
	char currentTeacher[MAX_TEACHER_NAME] = "";

	typedef struct {
		char name[MAX_TEACHER_NAME];
	} Teacher;

	Teacher teachers[100];
	int teacherCount = 0;
	int i;

	snprintf(filename, sizeof(filename), "%s_reviews.txt", department);
	file = fopen(filename, "r");
	if (!file) {
		printf("No reviews available for the %s department.\n", department);
		return;
	}

	while (fgets(line, sizeof(line), file)) {
		if (strncmp(line, "Teacher: ", 9) == 0) {
			char tempName[MAX_TEACHER_NAME];
			sscanf(line + 9, "%[^\n]", tempName);

			int exists = 0;
			for (i = 0; i < teacherCount; i++) {
				if (strcmp(teachers[i].name, tempName) == 0) {
					exists = 1;
					break;
				}
			}
			if (!exists && teacherCount < 100) {
				strcpy(teachers[teacherCount].name, tempName);
				teacherCount++;
			}
		}
	}
	fclose(file);

	if (teacherCount == 0) {
		printf("No teachers found in %s department.\n", department);
		return;
	}

	printf("\nAverage Ratings for Teachers in %s Department:\n", department);

	for (i = 0; i < teacherCount; i++) {
		int sum = 0, count = 0;

		file = fopen(filename, "r");
		if (!file) {
			printf("Error opening file.\n");
			return;
		}

		while (fgets(line, sizeof(line), file)) {
			if (strncmp(line, "Teacher: ", 9) == 0) {
				char tempName[MAX_TEACHER_NAME];
				sscanf(line + 9, "%[^\n]", tempName);

				if (strcmp(tempName, teachers[i].name) == 0) {
					fgets(line, sizeof(line), file);
					if (fgets(line, sizeof(line), file)) {
						int r;
						if (sscanf(line, "Rating: %d", &r) == 1) {
							sum += r;
							count++;
						}
					}
				}
			}
		}
		fclose(file);

		if (count > 0) {
			printf("%s: %.2f (%d review%s)\n", teachers[i].name, (float)sum / count, count, count > 1 ? "s" : "");
		} else {
			printf("%s: No ratings found\n", teachers[i].name);
		}
	}
}

// New Function: Find the highest rated teacher
void findHighestRatedTeacher(const char *department) {
	FILE *file;
	char filename[50], line[MAX_LINE_LENGTH];
	char currentTeacher[MAX_TEACHER_NAME] = "";
	int highestRatingSum = 0, highestRatingCount = 0;
	float highestAvg = -1.0f;
	char highestRatedTeacher[MAX_TEACHER_NAME] = "";

	typedef struct {
		char name[MAX_TEACHER_NAME];
	} Teacher;

	Teacher teachers[100];
	int teacherCount = 0;
	int i;

	snprintf(filename, sizeof(filename), "%s_reviews.txt", department);
	file = fopen(filename, "r");
	if (!file) {
		printf("No reviews available for the %s department.\n", department);
		return;
	}

	while (fgets(line, sizeof(line), file)) {
		if (strncmp(line, "Teacher: ", 9) == 0) {
			char tempName[MAX_TEACHER_NAME];
			sscanf(line + 9, "%[^\n]", tempName);

			int exists = 0;
			for (i = 0; i < teacherCount; i++) {
				if (strcmp(teachers[i].name, tempName) == 0) {
					exists = 1;
					break;
				}
			}
			if (!exists && teacherCount < 100) {
				strcpy(teachers[teacherCount].name, tempName);
				teacherCount++;
			}
		}
	}
	fclose(file);

	for (i = 0; i < teacherCount; i++) {
		int sum = 0, count = 0;

		file = fopen(filename, "r");
		if (!file) {
			printf("Error opening file.\n");
			return;
		}

		while (fgets(line, sizeof(line), file)) {
			if (strncmp(line, "Teacher: ", 9) == 0) {
				char tempName[MAX_TEACHER_NAME];
				sscanf(line + 9, "%[^\n]", tempName);

				if (strcmp(tempName, teachers[i].name) == 0) {
					fgets(line, sizeof(line), file);
					if (fgets(line, sizeof(line), file)) {
						int r;
						if (sscanf(line, "Rating: %d", &r) == 1) {
							sum += r;
							count++;
						}
					}
				}
			}
		}
		fclose(file);

		if (count > 0) {
			float avg = (float)sum / count;
			if (avg > highestAvg) {
				highestAvg = avg;
				strcpy(highestRatedTeacher, teachers[i].name);
				highestRatingSum = sum;
				highestRatingCount = count;
			}
		}
	}

	if (highestAvg >= 0) {
		printf("\nHighest Rated Teacher in %s Department:\n", department);
		printf("%s with average rating %.2f from %d review%s\n",
			   highestRatedTeacher, highestAvg, highestRatingCount,
			   highestRatingCount > 1 ? "s" : "");
	} else {
		printf("No ratings found for any teacher in %s department.\n", department);
	}
}

// Display main menu
void displayMenu() {
	printf("\nTeacher Review System\n");
	printf("1. Mechanical Department\n");
	printf("2. Civil Department\n");
	printf("3. Chemical Department\n");
	printf("4. Computer Science Department\n");
	printf("5. AIML Department\n");
	printf("6. Biotech Department\n");
	printf("7. Electrical Department\n");
	printf("8. Exit\n");
	printf("Enter your choice: ");
}

int main() {
	int choice;
	char teacherName[MAX_TEACHER_NAME], review[MAX_REVIEW_LENGTH], newReview[MAX_REVIEW_LENGTH];
	char department[50];
	int rating;

	while (1) {
		displayMenu();
		scanf("%d", &choice);
		getchar();

		if (choice == 8) {
			printf("Exiting the system. Goodbye!\n");
			break;
		}

		switch (choice) {
			case 1:
				strcpy(department, "Mechanical");
				break;
			case 2:
				strcpy(department, "Civil");
				break;
			case 3:
				strcpy(department, "Chemical");
				break;
			case 4:
				strcpy(department, "CompSci");
				break;
			case 5:
				strcpy(department, "AIML");
				break;
			case 6:
				strcpy(department, "Biotech");
				break;
			case 7:
				strcpy(department, "Electrical");
				break;
			default:
				printf("Invalid choice, please try again.\n");
				continue;
		}

		printf("\nSelect action:\n");
		printf("1. Add Review\n");
		printf("2. Display All Reviews\n");
		printf("3. Search Reviews by Teacher\n");
		printf("4. Edit Review\n");
		printf("5. Delete Reviews for Teacher\n");
		printf("6. Show Average Ratings for Teachers\n");
		printf("7. Find Highest Rated Teacher\n");
		printf("Enter your choice: ");
		int actionChoice;
		scanf("%d", &actionChoice);
		getchar();

		switch (actionChoice) {
			case 1:
				printf("Enter the teacher's name: ");
				fgets(teacherName, sizeof(teacherName), stdin);
				teacherName[strcspn(teacherName, "\n")] = 0;

				printf("Enter your review: ");
				fgets(review, sizeof(review), stdin);
				review[strcspn(review, "\n")] = 0;

				printf("Enter rating (1-5): ");
				scanf("%d", &rating);
				getchar();

				if (rating < 1 || rating > 5) {
					printf("Rating must be between 1 and 5.\n");
					break;
				}

				if (validateTeacherName(teacherName) && validateReview(review)) {
					writeReviewToFile(department, teacherName, review, rating);
				}
				break;

			case 2:
				displayReviews(department);
				break;

			case 3:
				printf("Enter the teacher's name to search reviews: ");
				fgets(teacherName, sizeof(teacherName), stdin);
				teacherName[strcspn(teacherName, "\n")] = 0;
				if (validateTeacherName(teacherName)) {
					searchReviewsByTeacher(department, teacherName);
				}
				break;

			case 4:
				printf("Enter the teacher's name to edit the review: ");
				fgets(teacherName, sizeof(teacherName), stdin);
				teacherName[strcspn(teacherName, "\n")] = 0;

				printf("Enter the new review: ");
				fgets(newReview, sizeof(newReview), stdin);
				newReview[strcspn(newReview, "\n")] = 0;

				printf("Enter new rating (1-5): ");
				scanf("%d", &rating);
				getchar();

				if (rating < 1 || rating > 5) {
					printf("Rating must be between 1 and 5.\n");
					break;
				}

				if (validateTeacherName(teacherName) && validateReview(newReview)) {
					editReview(department, teacherName, newReview, rating);
				}
				break;

			case 5:
				printf("Enter the teacher's name to delete reviews: ");
				fgets(teacherName, sizeof(teacherName), stdin);
				teacherName[strcspn(teacherName, "\n")] = 0;

				if (validateTeacherName(teacherName)) {
					deleteReview(department, teacherName);
				}
				break;

			case 6:
				calculateAverageRatings(department);
				break;

			case 7:
				findHighestRatedTeacher(department);
				break;

			default:
				printf("Invalid action.\n");
				break;
		}
	}

	return 0;
}

