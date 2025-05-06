#include <iostream>
#include <string>
#include <fstream>
using namespace std;

string username; // Global variable to store username
const int MAX_DOCTORS = 100;
const int MAX_APPOINTMENTS = 1000;
const int MAX_PATIENTS = 1000;
const int MAX_REVIEWS = 1000;
const int maxslotcount = 100;
int appointmentCount = 0;
int patientindex;
int doctorindex;
// constants for no user logged in
const string NO_LOGGED_IN_USERNAME = "";
const string NO_LOGGED_IN_USERTYPE = "";

struct Review
{
    int rating;
    string reviewerName;
    string reviewerType; // patent or doctor
};
struct TimeSlot
{
    int month;
    int day;
    int year;
    int beginHour;
    int beginMin;
    int endHour;
    int endMin;
    bool available;
};

struct appointment
{
    int day;
    int month;
    int year;
    int beginHour, endHour;
    int doctorId;
    int beginMin;
    int endMin;
    string patientName;
    string username;
};

struct Doctor
{
    string name;
    string username;
    int id;
    string specialty;
    string password;
    TimeSlot availabletimes[maxslotcount];
    int availablecount = 0;
    int patientcount = 0;
    appointment docAppointment[MAX_APPOINTMENTS];
    Review reviews[MAX_REVIEWS];
    int reviewCount;
};

struct Patient
{
    string name;
    string username;
    string password;
    int id;
    appointment patappointments[MAX_APPOINTMENTS];
    int appointmentCount = 0;
    int age;
    Review reviews[MAX_REVIEWS];
    int reviewCount;
};

// globals
appointment appointments[MAX_APPOINTMENTS];
Doctor doctors[MAX_DOCTORS];
Patient patients[MAX_PATIENTS];
int doctorcount = 0;
int patientcount = 0;

// who is logged in
string loggedin_username = NO_LOGGED_IN_USERNAME;
string loggedin_usertype = NO_LOGGED_IN_USERTYPE;

// ???? ?? ??? ???????
bool isValidDate(int day, int month, int year)
{
    if (year < 2025)
        return false;

    if (month < 1 || month > 12)
        return false;

    int maxDay;
    if (month == 2)
        maxDay = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ? 29 : 28;
    else if (month == 4 || month == 6 || month == 9 || month == 11)
        maxDay = 30;
    else
        maxDay = 31;

    return day >= 1 && day <= maxDay;
}
// ???? ?? ????? ??????? ??? ??? ???????
bool hasDuplicateDate(Doctor &doc, int day, int month, int year)
{
    for (int i = 0; i < doc.availablecount; i++)
    {
        if (doc.availabletimes[i].day == day &&
            doc.availabletimes[i].month == month &&
            doc.availabletimes[i].year == year)
        {
            return true;
        }
    }
    return false;
}
// ???? ????? ??????? ???????
void addTimeSlots(Doctor &doc)
{
    int day, month, year;
    int startHour, startMin, endHour, endMin;

    // ????? ???????
    while (true)
    {
        cout << "Enter available date (day month year): ";
        cin >> day >> month >> year;

        if (!isValidDate(day, month, year))
        {
            cout << "Invalid date. Please try again.\n";
            continue;
        }

        if (hasDuplicateDate(doc, day, month, year))
        {
            cout << "You already added time for this date.\n";
            return;
        }

        break;
    }

    // ????? ????? ?????? ???????
    while (true)
    {
        cout << "Enter start time (hour minute): ";
        cin >> startHour >> startMin;
        cout << "Enter end time (hour minute): ";
        cin >> endHour >> endMin;

        bool valid = false;
        if (startHour >= 0 && startHour <= 23 &&
            startMin >= 0 && startMin <= 59 &&
            endHour >= 0 && endHour <= 23 &&
            endMin >= 0 && endMin <= 59)
        {
            if (endHour > startHour || (endHour == startHour && endMin > startMin))
            {
                valid = true;
            }
        }

        if (valid)
            break;

        cout << "Invalid time range. Please try again.\n";
    }

    // ????? ??????? ??? ????
    int currentHour = startHour;
    int currentMin = startMin;

    while ((currentHour < endHour) || (currentHour == endHour && currentMin < endMin))
    {
        if (doc.availablecount >= maxslotcount)
        {
            cout << "Cannot add more slots.\n";
            break;
        }

        TimeSlot &slot = doc.availabletimes[doc.availablecount++];
        slot.day = day;
        slot.month = month;
        slot.year = year;
        slot.beginHour = currentHour;
        slot.beginMin = currentMin;

        // ???? ????? ???? ????
        int newHour = currentHour;
        int newMin = currentMin + 30;
        if (newMin >= 60)
        {
            newMin -= 60;
            newHour++;
        }
        slot.endHour = newHour;
        slot.endMin = newMin;
        slot.available = true;

        // ????? ????? ??????
        currentHour = newHour;
        currentMin = newMin;
    }

    cout << "All available slots added successfully.\n";
}

bool check_id_exists(int id)
{
    bool isid = false;

    for (int i = 0; i < doctorcount; i++)
    {
        if (doctors[i].id == id)
            isid = true;
    }
    for (int i = 0; i < patientcount; i++)
    {
        if (patients[i].id == id)
            isid = true;
    }
    return isid;
}

bool check_username_exists(string username)
{
    bool isuser = false;
    for (int i = 0; i < doctorcount; i++)
    {
        if (doctors[i].username == username)
            isuser = true;
    }
    for (int i = 0; i < patientcount; i++)
    {
        if (patients[i].username == username)
            isuser = true;
    }
    return isuser;
}

string get_password(bool confirmRequired = true)
{
    string password, confirm;
    do
    {
        cout << "Enter a password of at least 8 characters:\n";
        cin >> password;

        if (password.length() < 8)
        {
            cout << "Password is too short, try again.\n";
            continue;
        }

        if (confirmRequired)
        {
            cout << "Confirm password:\n";
            cin >> confirm;

            if (password != confirm)
            {
                cout << "Passwords do not match, try again.\n";
                continue;
            }
        }
    } while (password.length() < 8 || (confirmRequired && password != confirm));
    return password;
}

// Register doctor
void registerdoctor()
{
    if (doctorcount >= MAX_DOCTORS)
    {
        cout << "The maximum number of doctors has been reached\n";
        return;
    }

    Doctor dr;
    cout << "\n===== Welcome to the Doctors registration system =====\n";

    do
    {
        cout << "Enter id:\n";
        cin >> dr.id;
        if (check_id_exists(dr.id))
        {
            cout << "ID already exists, try again.\n";
        }
    } while (check_id_exists(dr.id));

    cout << "Enter name:\n";
    cin.ignore();
    getline(cin, dr.name);
    cout << "Enter the specialty:\n";
    getline(cin, dr.specialty);

    do
    {
        cout << "Enter username:\n";
        getline(cin, dr.username);
        if (check_username_exists(dr.username))
        {
            cout << "Username already in use, try again\n";
        }
    } while (check_username_exists(dr.username));

    dr.password = get_password();

    cout << "Enter number of available appointments (maximum " << MAX_APPOINTMENTS << "):\n";
    cin >> dr.availablecount;

    addTimeSlots(dr);
    dr.patientcount = 0;

    doctors[doctorcount++] = dr;

    cout << "Registration completed successfully.\n";
}

// Register patient
void registerPatient()
{
    if (patientcount >= MAX_PATIENTS)
    {
        cout << "The maximum number of patients has been reached\n";
        return;
    }

    Patient pt;
    cout << "\n======= Register patient =======\n";

    do
    {
        cout << "Enter id:\n";
        cin >> pt.id;
        if (check_id_exists(pt.id))
        {
            cout << "ID already exists, try again.\n";
        }
    } while (check_id_exists(pt.id));

    cout << "Enter name:\n";
    cin.ignore();
    getline(cin, pt.name);

    do
    {
        cout << "Enter username:\n";
        getline(cin, pt.username);
        if (check_username_exists(pt.username))
        {
            cout << "Username is already in use, try again\n";
        }
    } while (check_username_exists(pt.username));

    pt.password = get_password();

    cout << "Enter age:\n";
    cin >> pt.age;
    while (pt.age < 0 || pt.age > 120)
    {
        cout << "Invalid age. Please enter a valid age:\n";
        cin >> pt.age;
    }

    pt.appointmentCount = 0;

    patients[patientcount++] = pt;

    cout << "Registration completed successfully\n";
}
// list the doctors ,their specialization , and their id
void viewDoctors()
{
    cout << "Here is a list of the doctors:" << endl;
    cout << "Num.\tName\t\tspecialization\t\tID" << endl;
    cout << "--------------------------------------------------\n";
    for (int i = 0; i < MAX_DOCTORS; i++)
    {
        cout << i + 1 << "\t" << doctors[i].name << "\t\t" << doctors[i].specialty << "\t" << doctors[i].id << endl;
    }

    cout << "--------------------------------------------------\n";
}
void displayAvailableDoctors(Doctor doctors[], int numdoctors)
{
    int day, month, year;
    int beginHour, endHour;
    cout << "Enter date in format DD/MM/YYYY: ";
    cin >> day >> month >> year;
    if (!isValidDate(day, month, year))
    {
        cout << "Invalid date entered. Please try again.\n";
        return;
    }
    cout << "Enter appointment time (start,end): ";
    cin >> beginHour >> endHour;
    if (!(beginHour >= 0 && beginHour <= 23 && endHour > beginHour && endHour <= 24))
    {
        cout << "Invalid time range. Please try again.\n";
        return;
    }
    cout << "Available doctors on " << day << "/" << month << "/" << year << " during " << beginHour << "-" << endHour << ":\n";
    bool found = false;
    for (int i = 0; i < numdoctors; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (doctors[i].availabletimes[j].day == day &&
                doctors[i].availabletimes[j].month == month &&
                doctors[i].availabletimes[j].year == year &&
                doctors[i].availabletimes[j].beginHour == beginHour &&
                doctors[i].availabletimes[j].endHour == endHour &&
                doctors[i].availabletimes[j].available)
            {
                cout << "Doctor ID: " << doctors[i].id << " - Name: " << doctors[i].name << endl;
                found = true;
                break;
            }
        }
    }
    if (!found)
    {
        cout << "No doctors available at that time.\n";
    }
}

// ???? ??? ??????? ???????
void showAvailableSlots(Doctor &doc, int day, int month, int year)
{
    cout << "Available slots:\n";
    for (int i = 0; i < maxslotcount; i++)
    {
        if (doc.availabletimes[i].day == day &&
            doc.availabletimes[i].month == month &&
            doc.availabletimes[i].year == year &&
            doc.availabletimes[i].available)
        {
            cout << i + 1 << ". "
                 << doc.availabletimes[i].beginHour << ":00 to "
                 << doc.availabletimes[i].endHour << ":00" << endl;
        }
    }
}

void bookAppointment(Doctor doctors[], int numDoctors, Patient patients[], int &numPatients)
{
    string patientname;
    cout << "Enter your name: ";
    cin >> patientname;

    if (appointmentCount >= MAX_APPOINTMENTS)
    {
        cout << "No more appointments can be booked.\n";
        return;
    }

    int patientIndex = -1;
    for (int i = 0; i < numPatients; i++)
    {
        if (patients[i].username == patientname)
        {
            patientIndex = i;
            break;
        }
    }
    if (patientIndex == -1)
    {
        if (numPatients < MAX_PATIENTS)
        {
            patientIndex = numPatients;
            patients[numPatients].username = patientname;
            numPatients++;
        }
        else
        {
            cout << "No space left for more patients.\n";
            return;
        }
    }
    cout << "Available doctors:\n";
    viewDoctors();
    int doctorChoice;
    cout << "Enter the number of the doctor you want to book with: ";
    cin >> doctorChoice;
    if (doctorChoice < 1 || doctorChoice > numDoctors)
    {
        cout << "Invalid doctor choice.\n";
        return;
    }
    int docIndex = doctorChoice - 1;

    int day, month, year;
    cout << "Enter Date (DD/MM/YYYY): ";
    cin >> day >> month >> year;
    if (!isValidDate(day, month, year))
    {
        cout << "Invalid date entered. Please try again.\n";
        return;
    }
    // Check if this doctor has any available slots on this date
    bool hasAvailableSlot = false;
    for (int i = 0; i < maxslotcount; i++)
    {
        if (doctors[docIndex].availabletimes[i].day == day &&
            doctors[docIndex].availabletimes[i].month == month &&
            doctors[docIndex].availabletimes[i].year == year &&
            doctors[docIndex].availabletimes[i].available)
        {
            hasAvailableSlot = true;
            break;
        }
    }
    if (!hasAvailableSlot)
    {
        cout << "Sorry, there are no available appointments for this doctor on the selected date.\n";
        return;
    }
}
void editAppointment(Doctor doctors[], int numDoctors, Patient patients[], int numPatients)
{
    string patientname;
    cout << "Enter your name: ";
    cin >> patientname;
    int patientIndex = -1;
    for (int i = 0; i < numPatients; i++)
    {
        if (patients[i].username == patientname)
        {
            patientIndex = i;
            break;
        }
    }
    if (patientIndex == -1)
    {
        cout << "Patient not found.\n";
        return;
    }
    cout << "Your current appointments:\n";
    bool hasappointment = false;
    for (int i = 0; i < 5; i++)
    {
        if (patients[patientIndex].patappointments[i].beginHour != 0 && patients[patientIndex].patappointments[i].endHour != 0)
        {
            hasappointment = true;
            cout << i + 1 << " Date: " << patients[patientIndex].patappointments[i].day << "/" << patients[patientIndex].patappointments[i].month << "/" << patients[patientIndex].patappointments[i].year << " Time: " << patients[patientIndex].patappointments[i].beginHour << "-" << patients[patientIndex].patappointments[i].endHour << endl;
        }
    }
    if (!hasappointment)
    {
        cout << "You have no appointment to edit.\n";
        return;
    }
    int choice;
    cout << "Enter the appointment number you want to edit (1-5): ";
    cin >> choice;
    if (choice < 1 || choice > 5 || (patients[patientIndex].patappointments[choice - 1].beginHour == 0 && patients[patientIndex].patappointments[choice - 1].endHour == 0))
    {
        cout << "Invalid choice.\n";
        return;
    }
    appointment &appt = patients[patientIndex].patappointments[choice - 1];
    for (int i = 0; i < numDoctors; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (doctors[i].availabletimes[j].day == appt.day &&
                doctors[i].availabletimes[j].month == appt.month && doctors[i].availabletimes[j].year == appt.year &&
                doctors[i].availabletimes[j].beginHour == appt.beginHour &&
                doctors[i].availabletimes[j].endHour == appt.endHour)
            {
                doctors[i].availabletimes[j].available = true;
                break;
            }
        }
    }
    int newday, newmonth, newyear, newbeginHour, newendHour;
    cout << "Enter new date (DD/MM/YYYY): ";
    cin >> newday >> newmonth >> newyear;

    if (!isValidDate(newday, newmonth, newyear))
    {
        cout << "Invalid date entered.\n";
        return;
    }
    cout << "Available time slots for " << newday << "/" << newmonth << "/" << newyear << ":\n";
    bool foundAvailableSlot = false;
    int slotIndex = 1;
    for (int i = 0; i < numDoctors; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (doctors[i].availabletimes[j].day == newday &&
                doctors[i].availabletimes[j].month == newmonth &&
                doctors[i].availabletimes[j].year == newyear &&
                doctors[i].availabletimes[j].available)
            {
                cout << slotIndex << ". Doctor: " << doctors[i].name << " - Slot: "
                     << doctors[i].availabletimes[j].beginHour << ":00 to "
                     << doctors[i].availabletimes[j].endHour << ":00\n";
                foundAvailableSlot = true;
                slotIndex++;
            }
        }
    }
    if (!foundAvailableSlot)
    {
        cout << "No available time slots for this date.\n";
        return;
    }
    int slotChoice;
    cout << "Enter the number of the time slot you want to choose: ";
    cin >> slotChoice;
    if (slotChoice < 1)
    {
        cout << "Invalid choice.\n";
        return;
    }
    int selectedDoctorIndex = -1;
    int selectedSlotIndex = -1;
    slotIndex = 1;
    for (int i = 0; i < numDoctors; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (doctors[i].availabletimes[j].day == newday &&
                doctors[i].availabletimes[j].month == newmonth &&
                doctors[i].availabletimes[j].year == newyear &&
                doctors[i].availabletimes[j].available)
            {
                if (slotIndex == slotChoice)
                {
                    selectedDoctorIndex = i;
                    selectedSlotIndex = j;
                    break;
                }
                slotIndex++;
            }
        }
        if (selectedDoctorIndex != -1)
            break;
    }
    if (selectedDoctorIndex == -1)
    {
        cout << "The selected slot is no longer available.\n";
        return;
    }
    // book the selected slot
    doctors[selectedDoctorIndex].availabletimes[selectedSlotIndex].available = false;
    // update the appointment with new date and time
    appt.day = newday;
    appt.month = newmonth;
    appt.year = newyear;
    appt.beginHour = doctors[selectedDoctorIndex].availabletimes[selectedSlotIndex].beginHour;
    appt.endHour = doctors[selectedDoctorIndex].availabletimes[selectedSlotIndex].endHour;
    cout << "Appointment updated successfully!\n";
}

void registration()
{
    int choice;
    cout << "\n=== Registration ===" << endl;

    while (true)
    {
        cout << "Press 1 if you are a doctor" << endl;
        cout << "Press 2 if you are a patient" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1)
        {
            registerdoctor();
            break;
        }
        else if (choice == 2)
        {
            registerPatient();
            break;
        }
        else
        {
            cout << "Invalid option, please try again." << endl;
            continue;
        }
    }
}
string login()
{
    string username, password, newpassword;
    char choice;
    int attempts = 3;
    bool validchoice = false;
    cout << "===== LOGIN =====\n";

    while (true)
    {
        cout << "Enter username \n";
        cin.ignore();
        getline(cin, username);

        if (check_username_exists(username))
            break;

        char regChoice;
        cout << "Username not found. Do you want to register? (y/n): ";
        cin >> regChoice;

        if (regChoice == 'y' || regChoice == 'Y')
        {
            registration();
            continue;
        }
        else if (regChoice == 'n' || regChoice == 'N')
        {
            continue;
        }
        else
        {
            cout << "Invalid choice. Please enter 'y' or 'n'.\n";
        }
    }
    while (attempts--)
    {

        cout << "Enter password: ";
        password = get_password(false); // Without confirmation

        bool loginSuccessful = false;

        // Check doctors
        for (int i = 0; i < doctorcount; i++)
        {
            if (doctors[i].username == username && doctors[i].password == password)
            {
                loggedin_username = username;
                loggedin_usertype = "doctor";
                cout << "Welcome Dr. " << doctors[i].name << "!\n";
                loginSuccessful = true;
                break;
            }
        }

        // Only check patients if doctor login failed
        if (!loginSuccessful)
        {
            for (int i = 0; i < patientcount; i++)
            {
                if (patients[i].username == username && patients[i].password == password)
                {
                    loggedin_username = username;
                    loggedin_usertype = "patient";
                    cout << "Welcome " << patients[i].name << "!\n";
                    loginSuccessful = true;
                    break;
                }
            }
        }

        if (loginSuccessful)
        {
            return loggedin_usertype;
        }
        else if (attempts > 0)
        {
            cout << "Login failed. Please check your password. Remaining attempts: " << attempts << "\n";
        }
    }

    // Only reach here if all attempts failed
    cout << "All attempts used up.\n";
    while (!validchoice)
    {
        cout << "Do you want to change your password (y/n)? ";
        cin >> choice;
        if (choice == 'y' || choice == 'Y')
        {
            validchoice = true;
            newpassword = get_password(); // With confirmation
            for (int i = 0; i < doctorcount; i++)
            {
                if (doctors[i].username == username)
                {
                    doctors[i].password = newpassword;
                    cout << "Password updated successfully for doctor.\n";
                    loggedin_username = username;
                    loggedin_usertype = "doctor";
                    return loggedin_usertype;
                }
            }
            for (int i = 0; i < patientcount; i++)
            {
                if (patients[i].username == username)
                {
                    patients[i].password = newpassword;
                    cout << "Password updated successfully for patient.\n";
                    loggedin_username = username;
                    loggedin_usertype = "patient";
                    return loggedin_usertype;
                }
            }
        }
        else if (choice == 'n' || choice == 'N')
        {
            validchoice = true;
            cout << "Access denied!\n";
            return NO_LOGGED_IN_USERTYPE;
        }
        else
        {
            cout << "Invalid choice .Please enter 'y' or 'n' .\n";
        }
    }
    return NO_LOGGED_IN_USERTYPE;
}
void logout()
{
    if (loggedin_username != NO_LOGGED_IN_USERNAME)
    {
        loggedin_username = NO_LOGGED_IN_USERNAME;
        loggedin_usertype = NO_LOGGED_IN_USERTYPE;
        cout << "You are logged out.\n";
        cout << "Glad to serve you, see you again!\n";
    }
    else
    {
        cout << "No user logged in.\n";
    }
}
//
void viewBookedAppointments(const Doctor &doc)
{
    bool found = false;
    cout << "Booked appointments for Dr. " << doc.name << ":\n";

    for (int i = 0; i < doc.availablecount; i++)
    {
        const TimeSlot &slot = doc.availabletimes[i];

        if (!slot.available)
        {
            found = true;
            cout << "- Date: " << (slot.day < 10 ? "0" : "") << slot.day << "/"
                 << (slot.month < 10 ? "0" : "") << slot.month << "/"
                 << slot.year
                 << " | Time: "
                 << (slot.beginHour < 10 ? "0" : "") << slot.beginHour << ":"
                 << (slot.beginMin < 10 ? "0" : "") << slot.beginMin
                 << " - "
                 << (slot.endHour < 10 ? "0" : "") << slot.endHour << ":"
                 << (slot.endMin < 10 ? "0" : "") << slot.endMin
                 << endl;
        }
    }

    if (!found)
    {
        cout << "No booked appointments yet.\n";
    }
}
void editAvailableTime(Doctor doc[], int size, int docNum)
{

    int counB[1000] = {}; // تخزين المؤشر اذا وجد وقت
    int x = 0;            // مؤشر ومقياس لمعرفة اذا ماكان هنالك قت وجد
    bool found = false;

    for (int z = 0; z < size; z++)
    {
        if (doc[docNum].availabletimes[z].month != 0)
        {
            counB[x] = z;
            x++;
        }
    }

    if (x > 0)
    {

        do
        {
            cout << "Available times for doctor " << doc[docNum].name << ":\n";
            for (int yearI = 2023; yearI <= 2030; ++yearI)
            {
                for (int monI = 1; monI <= 12; ++monI)
                {
                    bool yPrinted = false;
                    bool mPrinted = false;
                    for (int dayI = 1; dayI <= 31; ++dayI)
                    {
                        bool dPrinted = false;
                        for (int i = 0; i < x; ++i)
                        {

                            if (doc[docNum].availabletimes[counB[i]].year == yearI && doc[docNum].availabletimes[counB[i]].month == monI && doc[docNum].availabletimes[counB[i]].day == dayI)
                            {

                                if (!yPrinted)
                                {
                                    cout << "\nyear: " << doc[docNum].availabletimes[counB[i]].year << endl;
                                    yPrinted = true;
                                }
                                if (!mPrinted)
                                {
                                    cout << "Month: " << monI << endl;
                                    mPrinted = true;
                                }
                                if (!dPrinted)
                                {
                                    cout << "  Day: " << dayI << endl;
                                    dPrinted = true;
                                }

                                cout << "    Time from ";
                                cout << doc[docNum].availabletimes[counB[i]].beginHour << ":" << doc[docNum].availabletimes[counB[i]].beginMin;
                                cout << " to ";
                                cout << doc[docNum].availabletimes[counB[i]].endHour << ":" << doc[docNum].availabletimes[counB[i]].endMin << endl;
                            }
                        }
                    }
                }
            }

            // ادخل ال وقت المراد تعديله

            int year, month, day, begH, begM, endH, endM;
            cout << "\nselect time from menu o change (year,month,day): \n";
            cin >> year >> month >> day;
            cout << "enter (begin hour,begin minutes,end hour,end minutes): \n";
            cin >> begH >> begM >> endH >> endM;
            // ابحث عن الوقت الذي تم ادخاله
            for (int i = 0; i < size; i++)
            {
                TimeSlot &newTime = doc[docNum].availabletimes[i];
                if (newTime.year == year && newTime.month == month && newTime.day == day && newTime.beginHour == begH && newTime.beginMin == begM && newTime.endHour == endH && newTime.endMin == endM)
                { // edit if found
                    cout << "\nEnter NEW year: ";
                    cin >> newTime.year;
                    cout << "Enter NEW month: ";
                    cin >> newTime.month;
                    cout << "Enter NEW day: ";
                    cin >> newTime.day;
                    cout << "Enter NEW begin hour: ";
                    cin >> newTime.beginHour;
                    cout << "Enter NEW begin minute: ";
                    cin >> newTime.beginMin;
                    newTime.endHour = newTime.beginHour;
                    newTime.endMin = newTime.beginMin + 30;

                    if (newTime.endMin >= 60)
                    {
                        newTime.endMin -= 60;
                        newTime.endHour += 1;
                    }

                    cout << "Time updated successfully!\n";
                    for (int yearI = 2023; yearI <= 2030; ++yearI)
                    {
                        for (int monI = 1; monI <= 12; ++monI)
                        {
                            bool yPrinted = false;
                            bool mPrinted = false;
                            for (int dayI = 1; dayI <= 31; ++dayI)
                            {
                                bool dPrinted = false;
                                for (int i = 0; i < x; ++i)
                                {

                                    if (doc[docNum].availabletimes[counB[i]].year == yearI && doc[docNum].availabletimes[counB[i]].month == monI && doc[docNum].availabletimes[counB[i]].day == dayI)
                                    {

                                        if (!yPrinted)
                                        {
                                            cout << "\nyear: " << doc[docNum].availabletimes[counB[i]].year << endl;
                                            yPrinted = true;
                                        }
                                        if (!mPrinted)
                                        {
                                            cout << "Month: " << monI << endl;
                                            mPrinted = true;
                                        }
                                        if (!dPrinted)
                                        {
                                            cout << "  Day: " << dayI << endl;
                                            dPrinted = true;
                                        }

                                        cout << "    Time from ";
                                        cout << doc[docNum].availabletimes[counB[i]].beginHour << ":" << doc[docNum].availabletimes[counB[i]].beginMin;
                                        cout << " to ";
                                        cout << doc[docNum].availabletimes[counB[i]].endHour << ":" << doc[docNum].availabletimes[counB[i]].endMin << endl;
                                    }
                                }
                            }
                        }
                    }
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                cout << "No matching time slot was found.\n";
            }
            char answer;
            cout << "do you want to change another time(y/n) : ";
            cin >> answer;
            if (answer == 'n' || answer == 'N')
            {
                break;
            }

        } while (true);
    }
    else
    {
        cout << "No time available for this doctor!\n";
    }
}

void viewPatient(Doctor doc[], int docNum)
{
    // int docNum ده هيتحدد عند تسجيل الدخول
    int coun[50] = {};
    int index = 0;
    for (int i = 0; i < 50; i++)
    {
        if (doc[docNum].docAppointment[i].username != "")
        {
            coun[index] = i;
            index++;
        }
    }
    if (index > 0)
    {
        for (int x = 0; x <= index; x++)
        {
            cout << "\npatinet num " << x + 1 << " : " << doc[docNum].docAppointment[coun[x]].username << endl;
            cout << "its time begin from : " << doc[docNum].docAppointment[coun[x]].beginHour << " : " << doc[docNum].docAppointment[coun[x]].beginMin
                 << " to " << doc[docNum].docAppointment[coun[x]].endHour << " : " << doc[docNum].docAppointment[coun[x]].endMin << endl;
        }
    }
    else
        cout << "no patient for this doctor yet !!\n";
}

// ???? ??? ???
// ???? ??? ???
void timeremove(Doctor &doc)
{
    cout << "\nAll Slots for Dr. " << doc.name << ":\n";
    if (doc.availablecount == 0)
    {
        cout << "No slots exist!\n";
        return;
    }

    // Display all slots
    for (int i = 0; i < doc.availablecount; i++)
    {
        cout << i + 1 << ". "
             << (doc.availabletimes[i].day < 10 ? "0" : "") << doc.availabletimes[i].day << "/"
             << (doc.availabletimes[i].month < 10 ? "0" : "") << doc.availabletimes[i].month << "/"
             << doc.availabletimes[i].year << " "
             << (doc.availabletimes[i].beginHour < 10 ? "0" : "") << doc.availabletimes[i].beginHour << ":"
             << (doc.availabletimes[i].beginMin < 10 ? "0" : "") << doc.availabletimes[i].beginMin << "-"
             << (doc.availabletimes[i].endHour < 10 ? "0" : "") << doc.availabletimes[i].endHour << ":"
             << (doc.availabletimes[i].endMin < 10 ? "0" : "") << doc.availabletimes[i].endMin
             << " [" << (doc.availabletimes[i].available ? "AVAILABLE" : "BOOKED") << "]\n";
    }

    // Get user choice
    char removeAnother;
    do
    {
        cout << "\nEnter slot number to remove (0 to cancel): ";
        int choice;
        cin >> choice;

        if (choice < 1 || choice > doc.availablecount)
        {
            cout << "Cancelled or invalid choice.\n";
            return;
        }

        // Remove slot
        for (int i = choice - 1; i < doc.availablecount - 1; i++)
        {
            doc.availabletimes[i] = doc.availabletimes[i + 1];
        }
        doc.availablecount--;
        cout << "Slot removed successfully!\n";

        if (doc.availablecount == 0)
        {
            cout << "No more slots remaining.\n";
            return;
        }

        cout << "Remove another slot? (y/n): ";
        cin >> removeAnother;

    } while (removeAnother == 'y' || removeAnother == 'Y');
}

// allow the doctor to edit user name and/or password
void editDoctorInfo(int doctorIndex)
{
    string newUsername, newPassword;
    int choice;
    cout << "Edit Doctor Information:" << endl;
    cout << "1.Change username" << endl;
    cout << "2.Change password" << endl;
    cout << "Please enter your choice: ";
    cin >> choice;
    switch (choice)
    {
    case 1:
    {
        cout << "Enter new username: ";
        cin >> newUsername;
        doctors[doctorIndex].username = newUsername;
        cout << "\n username updated to " << newUsername << " successfully! " << endl;
        break;
    }
    case 2:
    {
        cout << "Enter new password: ";
        cin >> newPassword;
        doctors[doctorIndex].password = newPassword;
        cout << "\n password updated successfully!" << endl;
        break;
    }

    default:
        cout << "Invalid choice, please try again " << endl;
    }
}
// allows the user to change thier username/age/password
void editPatientInfo(int patientIndex)
{

    int choice, newAge;
    string newUsername, newPassword;
    cout << "Edit patient Information:" << endl;
    cout << "1.Change username" << endl;
    cout << "2.Change password" << endl;
    cout << "3.edit age" << endl;
    cout << "Please enter your choice: ";
    cin >> choice;
    switch (choice)
    {
    case 1:
    {
        cout << "Enter new username: ";
        cin >> newUsername;
        patients[patientIndex].username = newUsername;
        cout << "\n username updated to " << newUsername << " successfully! " << endl;
        break;
    }
    case 2:
    {
        cout << "Enter new password: ";
        cin >> newPassword;
        patients[patientIndex].password = newPassword;
        cout << "\n password updated successfully!" << endl;
        break;
    }
    case 3:
    {

        // ???? ??? ???????? ????????
        cout << "Enter new age: ";
        cin >> newAge;
        patients[patientIndex].age = newAge;
        break;
    }

    default:
        cout << "Invalid choice, please try again " << endl;
    }
}

// Function to view patient's appointments
void viewPatientAppointments(int patientIndex)
{
    if (patients[patientIndex].appointmentCount == 0)
    {
        cout << "You have no appointments booked.\n";
        return;
    }

    cout << "Your appointments:\n";
    cout << "----------------------------------------\n";
    cout << "No.  Doctor           Date       Time\n";
    cout << "----------------------------------------\n";

    for (int i = 0; i < patients[patientIndex].appointmentCount; i++)
    {
        appointment apt = patients[patientIndex].patappointments[i];

        // Find the doctor
        string doctorName = "Unknown";
        for (int d = 0; d < doctorcount; d++)
        {
            if (doctors[d].id == apt.doctorId)
            {
                doctorName = doctors[d].name;
                break;
            }
        }

        cout << i + 1 << ".   " << doctorName << (apt.day < 10 ? "0" : "") << apt.day << "/"
             << (apt.month < 10 ? "0" : "") << apt.month << "/"
             << apt.year << "  ";

        cout << (apt.beginHour < 10 ? "0" : "") << apt.beginHour << ":00-"
             << (apt.endHour < 10 ? "0" : "") << apt.endHour << ":00\n";
    }
    cout << "----------------------------------------\n";
}

void deletePatientAppointment()
{

    cout << "Your appointments:\n";
    bool hasAppointments = false;
    for (int i = 0; i < patients[patientindex].appointmentCount; i++)
    {
        // Find matching doctor appointment
        for (int d = 0; d < doctorcount; d++)
        {
            for (int a = 0; a < doctors[d].patientcount; a++)
            {
                if (doctors[d].docAppointment[a].username == patients[patientindex].username &&
                    doctors[d].docAppointment[a].day == patients[patientindex].patappointments[i].day &&
                    doctors[d].docAppointment[a].month == patients[patientindex].patappointments[i].month &&
                    doctors[d].docAppointment[a].year == patients[patientindex].patappointments[i].year)
                {

                    hasAppointments = true;
                    cout << i + 1 << ". With Dr. " << doctors[d].name
                         << " (" << doctors[d].specialty << ")"
                         << " on " << patients[patientindex].patappointments[i].day << "/"
                         << patients[patientindex].patappointments[i].month << "/"
                         << patients[patientindex].patappointments[i].year
                         << " at " << doctors[d].docAppointment[a].beginHour << ":00-"
                         << doctors[d].docAppointment[a].endHour << ":00\n";
                    break;
                }
                else
                {
                    if (!hasAppointments)
                    {
                        cout << "You have no appointments to delete.\n";
                        return;
                    }
                }
            }
        }
    } // Get appointment to delete
    int choice;
    cout << "Enter the number of the appointment to delete: ";
    cin >> choice;

    if (choice < 1 || choice > patients[patientindex].appointmentCount)
    {
        cout << "Invalid choice.\n";
        return;
    }

    // Get date of appointment being deleted
    int bhour = patients[patientindex].patappointments[choice - 1].beginHour;
    int ehour = patients[patientindex].patappointments[choice - 1].endHour;
    int delDay = patients[patientindex].patappointments[choice - 1].day;
    int delMonth = patients[patientindex].patappointments[choice - 1].month;
    int delYear = patients[patientindex].patappointments[choice - 1].year;

    for (int i = choice - 1; i < patients[patientindex].appointmentCount - 1; i++)
    {
        patients[patientindex].patappointments[i] = patients[patientindex].patappointments[i + 1];
    }
    patients[patientindex].appointmentCount--;
    // Delete from doctor's schedule
    for (int d = 0; d < doctorcount; d++)
    {
        for (int a = 0; a < doctors[d].patientcount; a++)
        {
            if (doctors[d].docAppointment[a].username == patients[patientindex].username &&
                doctors[d].docAppointment[a].day == delDay &&
                doctors[d].docAppointment[a].month == delMonth &&
                doctors[d].docAppointment[a].year == delYear)
            {

                // REACTIVATE TIME SLOT
                for (int t = 0; t < doctors[d].availablecount; t++)
                {
                    if (doctors[d].availabletimes[t].day == delDay &&
                        doctors[d].availabletimes[t].month == delMonth &&
                        doctors[d].availabletimes[t].year == delYear &&
                        doctors[d].availabletimes[t].beginHour == doctors[d].docAppointment[a].beginHour)
                    {
                        doctors[d].availabletimes[t].available = true;
                        break;
                    }
                }

                for (int j = a; j < doctors[d].patientcount - 1; j++)
                {
                    doctors[d].docAppointment[j] = doctors[d].docAppointment[j + 1];
                }
                doctors[d].patientcount--;
                break;
            }
        }
    }

    cout << "Appointment deleted successfully.\n";
}

void clearPatientAppointments()
{

    if (patients[patientindex].appointmentCount == 0)
    {
        cout << "You have no appointments to clear.\n";
        return;
    }

    for (int d = 0; d < doctorcount; d++)
    {
        for (int a = 0; a < doctors[d].patientcount;)
        {
            if (doctors[d].docAppointment[a].username == patients[patientindex].username)
            {
                for (int t = 0; t < doctors[d].availablecount; t++)
                {
                    if (doctors[d].availabletimes[t].day == doctors[d].docAppointment[a].day &&
                        doctors[d].availabletimes[t].month == doctors[d].docAppointment[a].month &&
                        doctors[d].availabletimes[t].year == doctors[d].docAppointment[a].year &&
                        doctors[d].availabletimes[t].beginHour == doctors[d].docAppointment[a].beginHour)
                    {
                        doctors[d].availabletimes[t].available = true;
                        break;
                    }
                }
                for (int j = a; j < doctors[d].patientcount - 1; j++)
                {
                    doctors[d].docAppointment[j] = doctors[d].docAppointment[j + 1];
                }
                doctors[d].patientcount--;
            }
            else
            {
                a++;
            }
        }
    }

    for (int i = 0; i < patients[patientindex].appointmentCount; i++)
    {
        patients[patientindex].patappointments[i] = appointment();
    }
    patients[patientindex].appointmentCount = 0;

    cout << "All appointments cleared successfully.\n";
}
// review function for the user
int validateRating()
{

    int rate;
    while (true)
    {

        cin >> rate;
        if (rate > 5 || rate < 0)
        {
            cout << "Invalid input , please enter a number from 1 to 5 , 5 being the highest:";
            continue;
        }
        else
        {
            return rate;
        }
    }
}
void review()
{
    Review newReview;

    cout << "\nYour feedback is valuable to us!\n";
    if (loggedin_usertype == "doctor")
    {
        cout << "====Doctor Feedback====\n";
        cout << "rate from 1 to 5 , 5 being the highest:" << endl;
        cout << "How easy was it to navigate put hospital's website?" << endl;
        newReview.rating = validateRating();
        cout << "How fast did you find information you needed?" << endl;
        newReview.rating = validateRating();
        cout << "Is there medical inaccuracies or missing details that should be included?" << endl;
        newReview.rating = validateRating();
        newReview.reviewerName = doctors[doctorindex].name;
        newReview.reviewerType = "doctor";
    }
    else if (loggedin_usertype == "patient")
    {
        cout << "\n====Patient Feedback====\n";
        cout << "rate from 1 to 5 , 5 being the highest:" << endl;
        cout << "How easy was it to navigate put hospital's website?" << endl;
        newReview.rating = validateRating();
        cout << "How fast did you find information you needed?" << endl;
        cin >> newReview.rating;
        cout << "How easy was it to book online?" << endl;
        newReview.rating = validateRating();

        cout << "How likely would you recommend this webite to others? " << endl;
        newReview.rating = validateRating();
        newReview.reviewerName = patients[patientindex].name;
        newReview.reviewerType = "patient";
    }
}

// doctors' menu
void doctorMenu(int doctorIndex)
{
    int choice;
    char y;
    do
    {
        cout << "\tDOCTOR MENU\t" << endl;
        cout << "1.Add Available Time" << endl;
        cout << "2.Remove Available Time" << endl;
        cout << "3.Edit Available Time" << endl;
        cout << "4.View Patients with Appointments" << endl;
        cout << "5.Edit Personal Information" << endl;
        cout << "6.Feedback" << endl;
        cout << "7.logout" << endl;
        cout << "Please enter your choice: ";
        cin >> choice;
        switch (choice)
        {
        case 1:
            addTimeSlots(doctors[doctorIndex]);
            break;
        case 2:
            timeremove(doctors[doctorIndex]);
            break;
        case 3:
            editAvailableTime(doctors, MAX_DOCTORS, doctorIndex);
            break;
        case 4:
            viewPatient(doctors, doctorIndex);
            break;
        case 5:
            editDoctorInfo(doctorIndex);
            break;
        case 6:
        {
            review();

            break;
        }
        case 7:
        {
            logout();
            break;
        }

        default:
        {
            cout << "invalid choice , please try again " << endl;
            continue;
        }
        }
        cout << "do you want to return to menu(y/n):";
        cin >> y;

    } while (y == 'y' || y == 'Y');
}

// patient's menu
void patientMenu(int patientindex)
{
    int choice;
    char y;
    do
    {
        cout << "\tPATIENT MENU\t" << endl;
        cout << "1.view doctors" << endl;
        cout << "2.doctors avilable in certin time" << endl;
        cout << "3.book an appointment" << endl;
        cout << "4.view appoinments" << endl;
        cout << "5.edit appointment" << endl;
        cout << "6.delete appointment" << endl;
        cout << "7.clear appointment history" << endl;
        cout << "8.edit basic info" << endl;
        cout << "9.Feedback" << endl;
        cout << "10.logout" << endl;
        cout << "please enter your choice: ";
        cin >> choice;
        switch (choice)
        {
        case 1:
        {
            viewDoctors();
            break;
        }
        case 2:
        {
            displayAvailableDoctors(doctors, doctorcount);
            break;
        }
        case 3:
        {
            bookAppointment(doctors, doctorcount, patients, patientcount);
            break;
        }
        case 4:
        {
            viewPatientAppointments(patientindex);
            break;
        }
        case 5:
        {
            editAppointment(doctors, doctorcount, patients, patientcount);
            break;
        }
        case 6:
        {
            deletePatientAppointment();
            break;
        }
        case 7:
        {
            clearPatientAppointments();
            break;
        }
        case 8:
        {
            editPatientInfo(patientindex);
            break;
        }
        case 9:
        {
            review();
            break;
        }
        case 10:
        {
            logout();
            break;
        }
        default:
        {
            cout << "invalid choice,please try again" << endl;
            continue;
        }
        }
        cout << "do you want to return to menu(y/n):";
        cin >> y;
    } while (y == 'y' || y == 'Y');
}
// decide whether the user is a doctor or a patient and show the menu accordingly
void menu()

{
    if (loggedin_usertype == "doctor")
    {
        for (int i = 0; i < doctorcount; i++)
        {
            if (doctors[i].username == loggedin_username)
            {
                doctorMenu(i);
            }
        }
    }
    else if (loggedin_usertype == "patient")
    {
        for (int i = 0; i < patientcount; i++)
        {
            if (patients[i].username == loggedin_username)
            {
                patientMenu(i);
            }
        }
    }
}
void saveData()
{
    ofstream outfile("data.txt");
    if (!outfile.is_open())
    {
        cerr << "Error: Could not open data.txt for writing\n";
        return;
    }

    // Save doctors
    outfile << doctorcount << endl;  // First save the count for easier loading
    for (int i = 0; i < doctorcount && i < MAX_DOCTORS; i++)
    {
        // Basic info
        outfile << doctors[i].id << " "
                << doctors[i].name << " "
                << doctors[i].username << " "
                << doctors[i].password << " "
                << doctors[i].specialty << " ";

        // Count available time slots (not empty ones)
        int actualSlots = 0;
        for (int j = 0; j < maxslotcount; j++)
        {
            if (doctors[i].availabletimes[j].beginHour != 0 ||
                doctors[i].availabletimes[j].endHour != 0)
            {
                actualSlots++;
            }
        }
        outfile << actualSlots << " ";

        // Save only non-empty time slots
        for (int j = 0; j < maxslotcount; j++) {
            if (doctors[i].availabletimes[j].beginHour != 0 ||
                doctors[i].availabletimes[j].endHour != 0)
            {
                outfile << doctors[i].availabletimes[j].day << " "
                       << doctors[i].availabletimes[j].month << " "
                       << doctors[i].availabletimes[j].year << " "
                       << doctors[i].availabletimes[j].beginHour << " "
                       << doctors[i].availabletimes[j].beginMin << " "
                       << doctors[i].availabletimes[j].endHour << " "
                       << doctors[i].availabletimes[j].endMin << " "
                       << doctors[i].availabletimes[j].available << " ";
            }
        }

        // Save appointments for this doctor
        outfile << doctors[i].patientcount << " ";
        for (int j = 0; j < doctors[i].patientcount && j < MAX_APPOINTMENTS; j++)
        {
            outfile << doctors[i].docAppointment[j].patientName << " "
                    << doctors[i].docAppointment[j].day << " "
                    << doctors[i].docAppointment[j].month << " "
                    << doctors[i].docAppointment[j].year << " "
                    << doctors[i].docAppointment[j].beginHour << " "
                    << doctors[i].docAppointment[j].beginMin << " "
                    << doctors[i].docAppointment[j].endHour << " "
                    << doctors[i].docAppointment[j].endMin << " "
                    << doctors[i].docAppointment[j].username << " ";
        }

        // Count ACTUAL reviews (not empty ones)
        int actualReviews = 0;
        for (int j = 0; j < MAX_REVIEWS; j++)
        {
            if (!doctors[i].reviews[j].reviewerName.empty())
            {
                actualReviews++;
            }
        }
        outfile << actualReviews << " ";

        // Save only non-empty reviews
        for (int j = 0; j < MAX_REVIEWS; j++)
        {
            if (!doctors[i].reviews[j].reviewerName.empty())
            {
                outfile << doctors[i].reviews[j].rating << " "
                        << doctors[i].reviews[j].reviewerName << " "
                        << doctors[i].reviews[j].reviewerType << " ";
            }
        }
        outfile << "\n";
    }

    // Save patients separately
    outfile << patientcount << endl;
    for (int i = 0; i < patientcount && i < MAX_PATIENTS; i++)
    {
        outfile << patients[i].name << " "
                << patients[i].username << " "
                << patients[i].password << " "
                << patients[i].id << " "
                << patients[i].age << " "
                << patients[i].appointmentCount << " ";
                
        // Save patient appointments
        for (int j = 0; j < patients[i].appointmentCount && j < MAX_APPOINTMENTS; j++)
        {
            outfile << patients[i].patappointments[j].doctorId << " "
                    << patients[i].patappointments[j].day << " "
                    << patients[i].patappointments[j].month << " "
                    << patients[i].patappointments[j].year << " "
                    << patients[i].patappointments[j].beginHour << " "
                    << patients[i].patappointments[j].beginMin << " "
                    << patients[i].patappointments[j].endHour << " "
                    << patients[i].patappointments[j].endMin << " ";
        }
        
        // Save patient reviews
        outfile << patients[i].reviewCount << " ";
        for (int j = 0; j < patients[i].reviewCount && j < MAX_REVIEWS; j++)
        {
            outfile << patients[i].reviews[j].rating << " "
                    << patients[i].reviews[j].reviewerName << " "
                    << patients[i].reviews[j].reviewerType << " ";
        }
        outfile << "\n";
    }

    outfile.close();
    cout << "Data saved successfully. "
         << doctorcount << " doctors, "
         << patientcount << " patients stored.\n";
}

void loadData()
{
    ifstream infile("data.txt");
    if (!infile)
    {
        cerr << "Error opening file!\n";
        return;
    }

    // Reset counters
    doctorcount = patientcount = 0;

    // Load doctor count
    infile >> doctorcount;
    if (doctorcount > MAX_DOCTORS)
        doctorcount = MAX_DOCTORS;

    // Load doctors
    for (int i = 0; i < doctorcount; i++)
    {
        // Basic info
        infile >> doctors[i].id
               >> doctors[i].name
               >> doctors[i].username
               >> doctors[i].password
               >> doctors[i].specialty;

        // Load time slots
        int slotCount;
        infile >> slotCount;
        doctors[i].availablecount = slotCount;
        
        for (int j = 0; j < slotCount; j++)
        {
            infile >> doctors[i].availabletimes[j].day
                   >> doctors[i].availabletimes[j].month
                   >> doctors[i].availabletimes[j].year
                   >> doctors[i].availabletimes[j].beginHour
                   >> doctors[i].availabletimes[j].beginMin
                   >> doctors[i].availabletimes[j].endHour
                   >> doctors[i].availabletimes[j].endMin
                   >> doctors[i].availabletimes[j].available;
        }

        // Load appointments for this doctor
        infile >> doctors[i].patientcount;
        if (doctors[i].patientcount > MAX_APPOINTMENTS)
            doctors[i].patientcount = MAX_APPOINTMENTS;
            
        for (int j = 0; j < doctors[i].patientcount; j++)
        {
            infile >> doctors[i].docAppointment[j].patientName
                   >> doctors[i].docAppointment[j].day
                   >> doctors[i].docAppointment[j].month
                   >> doctors[i].docAppointment[j].year
                   >> doctors[i].docAppointment[j].beginHour
                   >> doctors[i].docAppointment[j].beginMin
                   >> doctors[i].docAppointment[j].endHour
                   >> doctors[i].docAppointment[j].endMin
                   >> doctors[i].docAppointment[j].username;
                   
            // Set the doctor ID in the appointment
            doctors[i].docAppointment[j].doctorId = doctors[i].id;
        }

        // Load reviews for this doctor
        infile >> doctors[i].reviewCount;
        if (doctors[i].reviewCount > MAX_REVIEWS)
            doctors[i].reviewCount = MAX_REVIEWS;
            
        for (int j = 0; j < doctors[i].reviewCount; j++)
        {
            infile >> doctors[i].reviews[j].rating
                   >> doctors[i].reviews[j].reviewerName
                   >> doctors[i].reviews[j].reviewerType;
        }
    }

    // Load patient count
    infile >> patientcount;
    if (patientcount > MAX_PATIENTS)
        patientcount = MAX_PATIENTS;
        
    // Load patients
    for (int i = 0; i < patientcount; i++)
    {
        infile >> patients[i].name
               >> patients[i].username
               >> patients[i].password
               >> patients[i].id
               >> patients[i].age
               >> patients[i].appointmentCount;
               
        if (patients[i].appointmentCount > MAX_APPOINTMENTS)
            patients[i].appointmentCount = MAX_APPOINTMENTS;
            
        // Load patient appointments
        for (int j = 0; j < patients[i].appointmentCount; j++)
        {
            infile >> patients[i].patappointments[j].doctorId
                   >> patients[i].patappointments[j].day
                   >> patients[i].patappointments[j].month
                   >> patients[i].patappointments[j].year
                   >> patients[i].patappointments[j].beginHour
                   >> patients[i].patappointments[j].beginMin
                   >> patients[i].patappointments[j].endHour
                   >> patients[i].patappointments[j].endMin;
                   
            // Set the patient username and name in the appointment
            patients[i].patappointments[j].username = patients[i].username;
            patients[i].patappointments[j].patientName = patients[i].name;
        }
        
        // Load patient reviews
        infile >> patients[i].reviewCount;
        if (patients[i].reviewCount > MAX_REVIEWS)
            patients[i].reviewCount = MAX_REVIEWS;
            
        for (int j = 0; j < patients[i].reviewCount; j++)
        {
            infile >> patients[i].reviews[j].rating
                   >> patients[i].reviews[j].reviewerName
                   >> patients[i].reviews[j].reviewerType;
        }
    }

    infile.close();
    cout << "Data loaded successfully. "
         << doctorcount << " doctors, "
         << patientcount << " patients loaded.\n";
}
// 
// void saveData()
// {
//     ofstream outfile("data.txt");
//     if (!outfile.is_open())
//     {
//         cerr << "Error: Could not open data.txt for writing\n";
//         return;
//     }
// 
//     // Save doctors
//     for (int i = 0; i < doctorcount && i < MAX_DOCTORS; i++)
//     {
//         // Basic info
//         outfile << doctors[i].id << " "
//                 << doctors[i].name << " "
//                 << doctors[i].username << " "
//                 << doctors[i].password << " "
//                 << doctors[i].specialty << " ";
// 
//         // Count available time slots (not empty ones)
//         int actualSlots = 0;
//         for (int j = 0; j < maxslotcount; j++)
//         {
//             if (doctors[i].availabletimes[j].beginHour != 0 ||
//                 doctors[i].availabletimes[j].endHour != 0)
//             {
//                 actualSlots++;
//             }
//         }
//         outfile << actualSlots << " ";
// 
//         // Save only non-empty time slots
//         for (int j = 0; j < doctors[doctorcount].availablecount; j++) {
//             outfile << doctors[doctorcount].availabletimes[j].day << " "
//                    << doctors[doctorcount].availabletimes[j].month << " "
//                    << doctors[doctorcount].availabletimes[j].year << " "
//                    << doctors[doctorcount].availabletimes[j].beginHour << " "
//                    << doctors[doctorcount].availabletimes[j].endHour;

 // TODO:   Missing beginMin and endMin fields @AhmedAlTayebC
// TODO:   accounting for the 'available' flag @AhmedAlTayebC

// 
//         // Save patients
//         for (int i = 0; i < patientcount && i < MAX_PATIENTS; i++)
//         {
//             outfile << patients[i].name << " "
//                     << patients[i].username << " "
//                     << patients[i].password << " "
//                     << patients[i].id << " "
//                     << patients[i].age << "\n";
//         }
// 
//         // Save appointments
//         outfile << doctors[i].patientcount << " ";
//         for (int j = 0; j < doctors[i].patientcount && j < MAX_APPOINTMENTS; j++)
//         {
//             outfile << doctors[i].docAppointment[j].patientName << " "
//                     << doctors[i].docAppointment[j].day << " "
//                     << doctors[i].docAppointment[j].month << " "
//                     << doctors[i].docAppointment[j].year << " "
//                     << doctors[i].docAppointment[j].beginHour << " "
//                     << doctors[i].docAppointment[j].endHour << " ";
//         }
// 
//         // Count ACTUAL reviews (not empty ones)
//         int actualReviews = 0;
//         for (int j = 0; j < MAX_REVIEWS; j++)
//         {
//             if (!doctors[i].reviews[j].reviewerName.empty())
//             {
//                 actualReviews++;
//             }
//         }
//         outfile << actualReviews << " ";
// 
//         // Save only non-empty reviews
//         for (int j = 0; j < MAX_REVIEWS; j++)
//         {
//             if (!doctors[i].reviews[j].reviewerName.empty())
//             {
//                 outfile << doctors[i].reviews[j].rating << " "
//                         << doctors[i].reviews[j].reviewerName << " "
//                         << doctors[i].reviews[j].reviewerType << " ";
//             }
//         }
//         outfile << "\n";
//     }
// 
//     outfile.close();
//     cout << "Data saved successfully. "
//          << doctorcount << " doctors, "
//          << patientcount << " patients stored.\n";
// }
// // void loadData()
// // {
//     ifstream infile("data.txt");
//     if (!infile)
//     {
//         cerr << "Error opening file!\n";
//         return;
//     }
// 
//     // Reset counters
//     doctorcount = patientcount = appointmentCount = 0;
// 
//     // Load doctors
//     while (infile >>
//            doctors[doctorcount].id >>
//            doctors[doctorcount].name >>
//            doctors[doctorcount].username >>
//            doctors[doctorcount].password >>
//            doctors[doctorcount].specialty >>
//            doctors[doctorcount].availablecount)
//     {
//         doctorcount++;
//         if (doctorcount == MAX_DOCTORS)
//             break;
//     }
// 
//     // Load patients
//     while (infile >>
//            patients[patientcount].name >>
//            patients[patientcount].username >>
//            patients[patientcount].password >>
//            patients[patientcount].id >>
//            patients[patientcount].age)
//     {
//         patientcount++;
//         if (patientcount == MAX_PATIENTS)
//             break;
//     }
// 
//     // Load appointments
//     while (
// 
//         infile >> appointments[appointmentCount].doctorId >>
//         appointments[appointmentCount].patientName >>
//         appointments[appointmentCount].day >>
//         appointments[appointmentCount].month >>
//         appointments[appointmentCount].year >>
//         appointments[appointmentCount].username >>
//         appointments[appointmentCount].beginHour >>
//         appointments[appointmentCount].endHour)
//     {
//         appointmentCount++;
//         if (appointmentCount == MAX_APPOINTMENTS)
//             break;
//     }
// 
//     infile >> doctors[doctorcount].reviewCount;
//     for (int j = 0; j < doctors[doctorcount].reviewCount; j++)
//     {
//         infile >> doctors[doctorcount].reviews[j].rating >> doctors[doctorcount].reviews[j].reviewerName >> doctors[doctorcount].reviews[j].reviewerType;
//     }
// 
//     cout << "data loaded !!!!" << endl;
//     infile.close();
// }
int main()
{
    loadData();

    int userchoice; // the user's choice
    cout << "\t\t\t____________\n\n\n";
    cout << "\t\t\t              welcome to login page             \n\n\n";
    cout << "\t\t\t__          MENU         ___\n\n\n";
    while (true)
    {

        cout << "\t Press 1 to Login                                " << endl;
        cout << "\t Press 2 to Register                             " << endl
             << " \t";
        cin >> userchoice;
        if (userchoice == 1 || userchoice == 2)
        {
            switch (userchoice)
            {
            case 1:
            {
                login();
                if (loggedin_username != NO_LOGGED_IN_USERNAME)
                {
                    menu();
                }
                break;
            }
            case 2:
            {
                registration();
                // After registration, ask to login
                cout << "Please login with your new account:" << endl;
                login();
                if (loggedin_username != NO_LOGGED_IN_USERNAME)
                {
                    menu();
                }
                break;
            }
            }
        }

        else
        {
            saveData();
            cout << "Please use a valid option from the menu." << endl;

            continue;
        }
    }
    saveData();
    return 0;
}