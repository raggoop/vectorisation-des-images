#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include "contour.h"



Point find_initial_pixel(Image I){
    UINT x,y;
	UINT L,H;
	Pixel_Point p1;
    Pixel_Point p2;
	L = I.L;
	H = I.H;
    p1.color = get_pixel_image(I,1,1);
    p1.p.x = 1;
    p1.p.y= 1;
    
	for(y=1; y<=H;y++){
		for(x=1;x<=L;x++){
			p1.color = get_pixel_image(I,x,y);
            p1.p.x = x;
            p1.p.y= y;
			p2.color = get_pixel_image(I,x,y-1);
            p2.p.x =x;
            p2.p.y= y-1;
            
            if(p1.color==NOIR && p2.color== BLANC){
                return p1.p;
            }
            
		}
        
	}

    Point p;
    p.x =-1;
    p.y =-1;
    return p;
}

void find_contour(Image I,Image *M,int *countToWhites,int *countSegments,Point initial_pixel,List_Multiple_Contours* multi_contours){
    //printf("L: %d, H: %d\n",I.L,I.H);
    
    Point inital_point;
    inital_point.x = initial_pixel.x-1;
    inital_point.y = initial_pixel.y-1;

    Point position;
    position.x = inital_point.x;
    position.y = inital_point.y;
    
    Robot r;
    initialize_Robot(&r,position,East);
    
    Orientation orientation = r.o;
    List_Point contour;
    contour = create_List_Point_vide();
    add_element_List_Point(&contour, position);
    //printf("%f,%f\n",position.x,position.y);
    

    bool flag = true;;
    while (flag){
        if(orientation == East){
           
              
            set_pixel_image(*M,position.x+1,position.y+1,0);
            

            (*countToWhites) ++;
        }
        move(&r);
        
        position.x = r.p.x;
        position.y = r.p.y;
        
        
        add_element_List_Point(&contour, position);
        
        orientation = new_orientation(position,I,&r);
        
        if( position.x == inital_point.x && position.y == inital_point.y && orientation == East ){
            flag = false;
        }
    }
    
    // printf("Nombre of white: %d\n",*countToWhites);
   
    // printf("Nombre de segment: %d\n",contour.size-1);
    //fprintf(file,"1\n");
    // write_contour_in_file(contour,file);
    // convert_to_eps(contour,file_eps,I.L,I.H);
    (*countSegments) = contour.size-1;
    add_next_Contour(multi_contours,contour);
    
}

Image create_image_mask(Image I, int *count){
    Image M = image_mask(I);
    UINT x,y;
	UINT L,H;
	L = M.L;
	H = M.H;
    Pixel_Point p1;
    Pixel_Point p2;
	for(y=1; y<=H;y++){
		for(x=1;x<=L;x++){
            p1.color = get_pixel_image(I,x,y);
            p1.p.x = x;
            p1.p.y= y;
			p2.color = get_pixel_image(I,x,y-1);
            p2.p.x =x;
            p2.p.y= y-1;
            
            if(p1.color==NOIR && p2.color== BLANC){
                set_pixel_image(M,x,y,1);
                (*count) ++;
            }
		}
	}
    return M;
}
void write_in_files(Image I,List_Multiple_Contours multi_contours, FILE* file, FILE* file_eps){
    
    fprintf(file,"%d\n",multi_contours.count);
    fprintf(file,"\n");
    fprintf(file_eps,"%%!PS−Adobe−3.0 EPSF−3.0\n");
	fprintf(file_eps,"%%%%BoundingBox: 0 -%d %d 0\n",I.H,I.L);
    Node_Multi_Contour* node;
    node = (Node_Multi_Contour *)malloc(sizeof(Node_Multi_Contour));
    node = multi_contours.head;

    while (node !=NULL)
    {
        write_contour_in_file(node->data,file);
        convert_to_eps(node->data,file_eps,I.L,I.H);
        node = node->next_contour;
    }
    fprintf(file_eps,"fill\n");
    fprintf(file_eps,"showpage\n");
    fclose(file);
    fclose(file_eps);
}


void find_multiple_contour(Image I,FILE *file,FILE *file_eps){
	UINT x,y;
	UINT L,H;
	L = I.L;
	H = I.H;
    int countBlacks = 0;
    int countToWhites = 0;
    int countContour = 0;
    int toltalPoints = 0;
    List_Multiple_Contours multi_contours;
    multi_contours = create_List_Multi_empty();
    Image M = create_image_mask(I,&countBlacks);
    int allSegments=0;
    int currSegmentsCount =0;

	Pixel_Point pixel;
	for(y=1; y<=H;y++){
		for(x=1;x<=L;x++){
			pixel.color = get_pixel_image(M,x,y);
		    if (pixel.color == 1){
				Point p;
                p.x = x;
                p.y = y;
                find_contour(I,&M,&countToWhites,&currSegmentsCount,p,&multi_contours);
                //ecrire_image(M);
                allSegments += currSegmentsCount;
                countContour++;
                
                if (countToWhites == countBlacks){
                    break;
                }
			}
		}
	}
    printf("Sum of all segments is : %d\n",allSegments);

    printf("Number of coutours : %d\n",multi_contours.count);
    write_in_files(I,multi_contours,file,file_eps);
        
}




