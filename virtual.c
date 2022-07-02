#include <stdio.h>
#include <stdlib.h>
#include "oslabs.h"

int process_page_access_fifo(struct PTE page_table[TABLEMAX], int *table_cnt, int page_number, 
                             int frame_pool[POOLMAX],int *frame_cnt, int current_timestamp);

int count_page_faults_fifo(struct PTE page_table[TABLEMAX],int table_cnt, int refrence_string[REFERENCEMAX], 
                           int reference_cnt, int frame_pool[POOLMAX],int frame_cnt);

int process_page_access_lru(struct PTE page_table[TABLEMAX], int *table_cnt, int page_number, int frame_pool[POOLMAX], 
                            int *frame_cnt, int current_timestamp);

int count_page_faults_lru(struct PTE page_table[TABLEMAX], int table_cnt, int refrence_string[REFERENCEMAX], 
                          int reference_cnt, int frame_pool[POOLMAX], int frame_cnt);

int process_page_access_lfu(struct PTE page_table[TABLEMAX], int *table_cnt, int page_number, int frame_pool[POOLMAX], 
                            int *frame_cnt, int current_timestamp);

int count_page_faults_lfu(struct PTE page_table[TABLEMAX], int table_cnt, int refrence_string[REFERENCEMAX], int reference_cnt, 
                          int frame_pool[POOLMAX], int frame_cnt);


int process_page_access_fifo(struct PTE page_table[TABLEMAX], int *table_cnt, int page_number, 
                             int frame_pool[POOLMAX],int *frame_cnt, int current_timestamp) {
                                int in_memory = 0; //If page being refrenced in memory, then this will be set to 1
                                int free_frame = 0; //Keeps track of any frames removed from the pool table
                                int selection = 0; //Keeps track of valid page index with smallest arrival_timestamp
                                int selected_frame_number = 0; //Frame number of selection
                                if(page_table[page_number].is_valid == 1) {
                                    in_memory = 1;
                                }
                                //Selects temporary valid page in memory to initialize selection index
                                for(int i = 0; i < *table_cnt; i++) {
                                    if(page_table[i].is_valid == 1) {
                                        selection = i;
                                        break;
                                    }
                                }
                                if(in_memory == 0) {
                                    //If not in memory and there are free frames in the frame_pool
                                    if(*frame_cnt > 0) {
                                        //Remove frame from pool to be insserted into page-table
                                        free_frame = frame_pool[*frame_cnt-1];
                                        (*frame_cnt)--;
                                        //Insert frame number into the page-table entry cooresponding to the logical page number
                                        page_table[page_number].is_valid = 1;
                                        page_table[page_number].frame_number = free_frame;
                                        page_table[page_number].arrival_timestamp = current_timestamp;
                                        page_table[page_number].last_access_timestamp = current_timestamp;
                                        page_table[page_number].reference_count = 1;
                                        return page_table[page_number].frame_number;
                                    } else { //If not in memory and there are no free frames in the frame_pool
                                        for(int i = 0; i < *table_cnt; i++) {
                                            if(page_table[i].arrival_timestamp < page_table[selection].arrival_timestamp && page_table[i].is_valid == 1) {
                                                selection = i;
                                                selected_frame_number = page_table[i].frame_number;
                                            }
                                        }
                                        //Set selected page to be IV:0, FN:-1, ATS:-1, LATS:-1, RC:-1
                                        page_table[selection].is_valid = 0;
                                        page_table[selection].frame_number = -1;
                                        page_table[selection].arrival_timestamp = -1;
                                        page_table[selection].last_access_timestamp = -1;
                                        page_table[selection].reference_count = -1;
                                        //Sets frame number of referenced page as the page we freed above and update other attributes accordingly
                                        page_table[page_number].is_valid = 1;
                                        page_table[page_number].frame_number = selected_frame_number;
                                        page_table[page_number].arrival_timestamp = current_timestamp;
                                        page_table[page_number].last_access_timestamp = current_timestamp;
                                        page_table[page_number].reference_count = 1;
                                        return page_table[page_number].frame_number;
                                    }
                                }
                                else { //If page being referenced is in memory
                                    for(int i = 0; i < *table_cnt; i++) {
                                        //If valid, update last_access_timestamp and reference_count, then return the page frame
                                        if(page_table[i].is_valid == 1) {
                                            page_table[i].last_access_timestamp = current_timestamp;
                                            page_table[i].reference_count += 1;
                                            return page_table[i].frame_number;
                                        }
                                    }
                                }
                             }

int count_page_faults_fifo(struct PTE page_table[TABLEMAX], int table_cnt, int refrence_string[REFERENCEMAX], 
                           int reference_cnt, int frame_pool[POOLMAX],int frame_cnt) {
                            int page_faults = 0; //Keeps track of page faults
                            int time_stamp = 1;
                            int selection = 0; //Keeps track of valid page index with smallest arrival_timestamp
                            int selected_frame_number = 0; //Frame number of selection
                            //Iterate through reference string to use as page_number below
                            for(int i = 0; i < reference_cnt; i++) {
                                time_stamp++; //Inccrements at the start of the loop, starts from 1
                                int in_memory = 0; //If page being refrenced in memory, then this will be set to 1
                                int page_number = refrence_string[i]; //Sets current page_number
                                int free_frame = 0; //Keeps track of any frames removed from the pool table
                                //Selects temporary valid page in memory to initialize selection index
                                for(int n = 0; n < table_cnt; n++) {
                                    if(page_table[n].is_valid == 1) {
                                        selection = n;
                                        break;
                                    }
                                }
                                //If page being refrenced in memory is valid, then this will be set to 1
                                if(page_table[page_number].is_valid == 1) {
                                    in_memory = 1;
                                }
                                //If page being referenced is in memory
                                if(in_memory == 1) {
                                    //If valid, update last_access_timestamp and reference_count, then return the page frame
                                    page_table[page_number].last_access_timestamp = time_stamp;
                                    page_table[page_number].reference_count += 1;
                                } else {
                                    //If not in memory and there are free frames in the frame_pool
                                    if(frame_cnt > 0) {
                                        //Remove frame from pool to be inserted into page-table
                                        free_frame = frame_pool[frame_cnt-1];
                                        frame_cnt--;
                                        //Insert frame number into the page-table entry cooresponding to the logical page number
                                        page_table[page_number].is_valid = 1;
                                        page_table[page_number].frame_number = free_frame+1;
                                        page_table[page_number].arrival_timestamp = time_stamp;
                                        page_table[page_number].last_access_timestamp = time_stamp;
                                        page_table[page_number].reference_count = 1;
                                        page_faults++; //Count page reference as page fault
                                    } else { //If not in memory and there are no free frames in the frame_pool
                                        for(int n = 0; n < table_cnt; n++) {
                                            if(page_table[n].arrival_timestamp < page_table[selection].arrival_timestamp && page_table[n].is_valid == 1) {
                                                selection = n;
                                                selected_frame_number = page_table[n].frame_number;
                                            } else {
                                                selected_frame_number = page_table[selection].frame_number;
                                            }
                                        }
                                        //Set selected page to be IV:0, FN:-1, ATS:-1, LATS:-1, RC:-1
                                        page_table[selection].is_valid = 0;
                                        page_table[selection].frame_number = -1;
                                        page_table[selection].arrival_timestamp = -1;
                                        page_table[selection].last_access_timestamp = -1;
                                        page_table[selection].reference_count = -1;
                                        //Sets frame number of referenced page as the page we freed above and update other attributes accordingly
                                        page_table[page_number].is_valid = 1;
                                        page_table[page_number].frame_number = selected_frame_number;
                                        page_table[page_number].arrival_timestamp = time_stamp;
                                        page_table[page_number].last_access_timestamp = time_stamp;
                                        page_table[page_number].reference_count = 1;
                                        page_faults++; //Count page reference as page fault
                                    } 
                                }
                            }
                            return page_faults;
                        }

int process_page_access_lru(struct PTE page_table[TABLEMAX], int *table_cnt, int page_number, int frame_pool[POOLMAX], 
                            int *frame_cnt, int current_timestamp) {
                                int in_memory = 0; //If page being refrenced in memory, then this will be set to 1
                                int free_frame = 0; //Keeps track of any frames removed from the pool table
                                int selection = 0; //Keeps track of valid page index with smallest arrival_timestamp
                                int selected_frame_number = 0; //Frame number of selection
                                if(page_table[page_number].is_valid == 1) {
                                    in_memory = 1;
                                }
                                //Selects temporary valid page in memory to initialize selection index
                                for(int i = 0; i < *table_cnt; i++) {
                                    if(page_table[i].is_valid == 1) {
                                        selection = i;
                                        break;
                                    }
                                }
                                if(in_memory == 0) {
                                    //If not in memory and there are free frames in the frame_pool
                                    if(*frame_cnt > 0) {
                                        //Remove frame from pool to be insserted into page-table
                                        free_frame = frame_pool[*frame_cnt-1];
                                        (*frame_cnt)--;
                                        //Insert frame number into the page-table entry cooresponding to the logical page number
                                        page_table[page_number].is_valid = 1;
                                        page_table[page_number].frame_number = free_frame;
                                        page_table[page_number].arrival_timestamp = current_timestamp;
                                        page_table[page_number].last_access_timestamp = current_timestamp;
                                        page_table[page_number].reference_count = 1;
                                        return page_table[page_number].frame_number;
                                    } else { ////If not in memory and there are no free frames in the frame_pool
                                        for(int i = 0; i < *table_cnt; i++) {
                                            if(page_table[i].last_access_timestamp < page_table[selection].last_access_timestamp && page_table[i].is_valid == 1) {
                                                selection = i;
                                                selected_frame_number = page_table[i].frame_number;
                                            }
                                        }
                                        //Set selected page to be IV:0, FN:-1, ATS:-1, LATS:-1, RC:-1
                                        page_table[selection].is_valid = 0;
                                        page_table[selection].frame_number = -1;
                                        page_table[selection].arrival_timestamp = -1;
                                        page_table[selection].last_access_timestamp = -1;
                                        page_table[selection].reference_count = -1;
                                        //Sets frame number of referenced page as the page we freed above and update other attributes accordingly
                                        page_table[page_number].is_valid = 1;
                                        page_table[page_number].frame_number = selected_frame_number;
                                        page_table[page_number].arrival_timestamp = current_timestamp;
                                        page_table[page_number].last_access_timestamp = current_timestamp;
                                        page_table[page_number].reference_count = 1;
                                        return page_table[page_number].frame_number;
                                    }
                                }
                                else { //If page being referenced is in memory
                                    for(int i = 0; i < *table_cnt; i++) {
                                        //If valid, update last_access_timestamp and reference_count, then return the page frame
                                        if(page_table[i].is_valid == 1) {
                                            page_table[i].last_access_timestamp = current_timestamp;
                                            page_table[i].reference_count += 1;
                                            return page_table[i].frame_number;
                                        }
                                    }
                                }
                            }

int count_page_faults_lru(struct PTE page_table[TABLEMAX], int table_cnt, int refrence_string[REFERENCEMAX], 
                          int reference_cnt, int frame_pool[POOLMAX], int frame_cnt) {
                            int page_faults = 0; //Keeps track of page faults
                            int time_stamp = 1;
                            int selection = 0; //Keeps track of valid page index with smallest arrival_timestamp
                            int selected_frame_number = 0; //Frame number of selection
                            //Iterate through reference string to use as page_number below
                            for(int i = 0; i < reference_cnt; i++) {
                                time_stamp++; //Increments at the start of the loop, starts from 1
                                int in_memory = 0; //If page being refrenced in memory, then this will be set to 1
                                int page_number = refrence_string[i]; //Sets current page_number
                                int free_frame = 0; //Keeps track of any frames removed from the pool table
                                //Selects temporary valid page in memory to initialize selection index
                                for(int n = 0; n < table_cnt; n++) {
                                    if(page_table[n].is_valid == 1) {
                                        selection = n;
                                        break;
                                    }
                                }
                                //If page being refrenced in memory is valid, then this will be set to 1
                                if(page_table[page_number].is_valid == 1) {
                                    in_memory = 1;
                                }
                                //If page being referenced is in memory
                                if(in_memory == 1) {
                                    //If valid, update last_access_timestamp and reference_count, then return the page frame
                                    page_table[page_number].last_access_timestamp = time_stamp;
                                    page_table[page_number].reference_count += 1;
                                } else {
                                    //If not in memory and there are free frames in the frame_pool
                                    if(frame_cnt > 0) {
                                        //Remove frame from pool to be inserted into page-table
                                        free_frame = frame_pool[frame_cnt-1];
                                        frame_cnt--;
                                        //Insert frame number into the page-table entry cooresponding to the logical page number
                                        page_table[page_number].is_valid = 1;
                                        page_table[page_number].frame_number = free_frame+1;
                                        page_table[page_number].arrival_timestamp = time_stamp;
                                        page_table[page_number].last_access_timestamp = time_stamp;
                                        page_table[page_number].reference_count = 1;
                                        page_faults++; //Count page reference as page fault
                                    } else { //If not in memory and there are no free frames in the frame_pool
                                        for(int i = 0; i < table_cnt; i++) {
                                            if(page_table[i].last_access_timestamp < page_table[selection].last_access_timestamp && page_table[i].is_valid == 1) {
                                                selection = i;
                                                selected_frame_number = page_table[i].frame_number;
                                            } else {
                                                selected_frame_number = page_table[selection].frame_number;
                                            }
                                        }
                                        //Set selected page to be IV:0, FN:-1, ATS:-1, LATS:-1, RC:-1
                                        page_table[selection].is_valid = 0;
                                        page_table[selection].frame_number = -1;
                                        page_table[selection].arrival_timestamp = -1;
                                        page_table[selection].last_access_timestamp = -1;
                                        page_table[selection].reference_count = -1;
                                        //Sets frame number of referenced page as the page we freed above and update other attributes accordingly
                                        page_table[page_number].is_valid = 1;
                                        page_table[page_number].frame_number = selected_frame_number;
                                        page_table[page_number].arrival_timestamp = time_stamp;
                                        page_table[page_number].last_access_timestamp = time_stamp;
                                        page_table[page_number].reference_count = 1;
                                        page_faults++; //Count page reference as page fault
                                    } 
                                }
                            }
                            return page_faults;
                        }

int process_page_access_lfu(struct PTE page_table[TABLEMAX], int *table_cnt, int page_number, int frame_pool[POOLMAX], 
                            int *frame_cnt, int current_timestamp) {
                                int in_memory = 0; //If page being refrenced in memory, then this will be set to 1
                                int free_frame = 0; //Keeps track of any frames removed from the pool table
                                int selection = 0; //Keeps track of valid page index with smallest arrival_timestamp
                                int selected_frame_number = 0; //Frame number of selection
                                if(page_table[page_number].is_valid == 1) {
                                    in_memory = 1;
                                }
                                //Selects temporary valid page in memory to initialize selection index
                                for(int i = 0; i < *table_cnt; i++) {
                                    if(page_table[i].is_valid == 1) {
                                        selection = i;
                                        break;
                                    }
                                }
                                if(in_memory == 0) {
                                    //If not in memory and there are free frames in the frame_pool
                                    if(*frame_cnt > 0) {
                                        //Remove frame from pool to be insserted into page-table
                                        free_frame = frame_pool[*frame_cnt-1];
                                        (*frame_cnt)--;
                                        //Insert frame number into the page-table entry cooresponding to the logical page number
                                        page_table[page_number].is_valid = 1;
                                        page_table[page_number].frame_number = free_frame;
                                        page_table[page_number].arrival_timestamp = current_timestamp;
                                        page_table[page_number].last_access_timestamp = current_timestamp;
                                        page_table[page_number].reference_count = 1;
                                        return page_table[page_number].frame_number;
                                    } else { ////If not in memory and there are no free frames in the frame_pool
                                        for(int i = 0; i < *table_cnt; i++) {
                                            if(page_table[i].reference_count == page_table[selection].reference_count && page_table[i].is_valid == 1) {
                                                if(page_table[i].arrival_timestamp < page_table[selection].arrival_timestamp) {
                                                    selection = i;
                                                    selected_frame_number = page_table[i].frame_number;
                                                } else {
                                                    selected_frame_number = page_table[selection].frame_number;
                                                }
                                            } else if(page_table[i].reference_count < page_table[selection].reference_count && page_table[i].is_valid == 1) {
                                                selection = i;
                                                selected_frame_number = page_table[i].frame_number;
                                            }
                                        }
                                        //Set selected page to be IV:0, FN:-1, ATS:-1, LATS:-1, RC:-1
                                        page_table[selection].is_valid = 0;
                                        page_table[selection].frame_number = -1;
                                        page_table[selection].arrival_timestamp = -1;
                                        page_table[selection].last_access_timestamp = -1;
                                        page_table[selection].reference_count = -1;
                                        //Sets frame number of referenced page as the page we freed above and update other attributes accordingly
                                        page_table[page_number].is_valid = 1;
                                        page_table[page_number].frame_number = selected_frame_number;
                                        page_table[page_number].arrival_timestamp = current_timestamp;
                                        page_table[page_number].last_access_timestamp = current_timestamp;
                                        page_table[page_number].reference_count = 1;
                                        return page_table[page_number].frame_number;
                                    }
                                }
                                else { //If page being referenced is in memory
                                    for(int i = 0; i < *table_cnt; i++) {
                                        //If valid, update last_access_timestamp and reference_count, then return the page frame
                                        if(page_table[i].is_valid == 1) {
                                            page_table[i].last_access_timestamp = current_timestamp;
                                            page_table[i].reference_count += 1;
                                            return page_table[i].frame_number;
                                        }
                                    }
                                }
                            }

int count_page_faults_lfu(struct PTE page_table[TABLEMAX], int table_cnt, int refrence_string[REFERENCEMAX], int reference_cnt, 
                          int frame_pool[POOLMAX], int frame_cnt) {
                            int page_faults = 0; //Keeps track of page faults
                            int time_stamp = 0;
                            int selection = 0; //Keeps track of valid page index with smallest arrival_timestamp
                            int selected_frame_number = 0; //Frame number of selection
                            //Iterate through reference string to use as page_number below
                            for(int i = 0; i < reference_cnt; i++) {
                                time_stamp++; //Inccrements at the start of the loop, starts from 1
                                int in_memory = 0; //If page being refrenced in memory, then this will be set to 1
                                int page_number = refrence_string[i]; //Sets current page_number
                                int free_frame = 0; //Keeps track of any frames removed from the pool table
                                //Selects temporary valid page in memory to initialize selection index
                                for(int n = 0; n < table_cnt; n++) {
                                    if(page_table[n].is_valid == 1) {
                                        selection = n;
                                        break;
                                    }
                                }
                                //If page being refrenced in memory is valid, then this will be set to 1
                                if(page_table[page_number].is_valid == 1) {
                                    in_memory = 1;
                                }
                                //If page being referenced is in memory
                                if(in_memory == 1) {
                                    //If valid, Update last_access_timestamp and reference_count, then return the page frame
                                    page_table[page_number].last_access_timestamp = time_stamp;
                                    page_table[page_number].reference_count += 1;
                                } else {
                                    //If not in memory and there are free frames in the frame_pool
                                    if(frame_cnt > 0) {
                                        //Remove frame from pool to be inserted into page-table
                                        free_frame = frame_pool[frame_cnt-1];
                                        frame_cnt--;
                                        //Insert frame number into the page-table entry cooresponding to the logical page number
                                        page_table[page_number].is_valid = 1;
                                        page_table[page_number].frame_number = free_frame+1;
                                        page_table[page_number].arrival_timestamp = time_stamp;
                                        page_table[page_number].last_access_timestamp = time_stamp;
                                        page_table[page_number].reference_count = 1;
                                        page_faults++; //Count page reference as page fault
                                    } else { ////If not in memory and there are no free frames in the frame_pool
                                        for(int i = 0; i < table_cnt; i++) {
                                            if(page_table[i].reference_count == page_table[selection].reference_count && page_table[i].is_valid == 1) {
                                                if(page_table[i].arrival_timestamp < page_table[selection].arrival_timestamp) {
                                                    selection = i;
                                                    selected_frame_number = page_table[i].frame_number;
                                                } else {
                                                    selected_frame_number = page_table[selection].frame_number;
                                                }
                                            } else if(page_table[i].reference_count < page_table[selection].reference_count && page_table[i].is_valid == 1) {
                                                selection = i;
                                                selected_frame_number = page_table[i].frame_number;
                                            }
                                        }
                                        //Set selected page to be IV:0, FN:-1, ATS:-1, LATS:-1, RC:-1
                                        page_table[selection].is_valid = 0;
                                        page_table[selection].frame_number = -1;
                                        page_table[selection].arrival_timestamp = -1;
                                        page_table[selection].last_access_timestamp = -1;
                                        page_table[selection].reference_count = -1;
                                        //Sets frame number of referenced page as the page we freed above and update other attributes accordingly
                                        page_table[page_number].is_valid = 1;
                                        page_table[page_number].frame_number = selected_frame_number;
                                        page_table[page_number].arrival_timestamp = time_stamp;
                                        page_table[page_number].last_access_timestamp = time_stamp;
                                        page_table[page_number].reference_count = 1;
                                        page_faults++; //Count page reference as page fault
                                    } 
                                }
                            }
                            return page_faults;
                        }