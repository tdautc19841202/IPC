#ifndef __TUYA_IPC_AI_FACE_DETECT_H__
#define __TUYA_IPC_AI_FACE_DETECT_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "tuya_ipc_cloud_storage.h"


#define FACE_PIC_MAX_SIZE (128*1024)


/*********************************************************************************
* TUYA_AI_FACE_UPLOAD_PARA
* is_strang_flag	    upload face type, 0 if is strange fece; 1 if is familiar
* face_score	        face scores, necessary when upload strange face
* faceid	            unique face id, necessary when upload familiar face
* face_jpeg_data	    Slower but high quality
* face_jpeg_data_size		Faster but low quality
* scene_jpeg_data	    Slower but high quality
* scene_jpeg_data_size		lower but high quality
**********************************************************************************/
typedef struct
{
    INT_T       is_strang_flag;
    INT_T       face_score;
    INT_T       face_id;

    CHAR_T*     face_jpeg_data;
    INT_T       face_jpeg_data_size;
    
    CHAR_T*     scene_jpeg_data;
    INT_T       scene_jpeg_data_size;
    
}TUYA_AI_FACE_UPLOAD_PARA;

typedef struct
{
    INT_T       face_id;
    CHAR_T      face_jpeg_data[FACE_PIC_MAX_SIZE];
    INT_T       face_jpeg_data_size;
}TUYA_AI_FACE_DATA;

typedef VOID (*IPC_AI_FACE_DETECT_CB)(IN CONST CHAR_T* action, IN CONST CHAR_T* face_id_list);
typedef VOID (*IPC_AI_FACE_DOWNLOAD_PIC_CB)(OUT PVOID_T callback_data);

/**
 * \fn OPERATE_RET tuya_ipc_ai_face_detect_storage_init 
 * \brief 
 * \     Init ai face detect. 
 * \     callback to set ai_cfg of callback[del add update]
 * \     callback to set pic download of callback[download pic circularly]
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_ai_face_detect_storage_init(IPC_AI_FACE_DETECT_CB cb,IPC_AI_FACE_DOWNLOAD_PIC_CB pic_download_cb);

/**
 * \fn OPERATE_RET tuya_ipc_ai_face_signed_report
 * \brief device upload signed face pic to service
 * \return OPERATE_RET
 */
OPERATE_RET tuya_ipc_ai_face_signed_report(TUYA_AI_FACE_UPLOAD_PARA * ai_face_image);

/**
 * \fn OPERATE_RET tuya_ipc_ai_face_new_report  
 * \brief 
 * device upload new face pic to service
 * \return OPERATE_RET0
 */
OPERATE_RET tuya_ipc_ai_face_new_report(TUYA_AI_FACE_UPLOAD_PARA * ai_face_image, OUT UINT_T *face_id);

/*********************************************************************************
* tuya_ipc_ai_face_update_cloud_face
* this func used to update all the AI face picture in the cloud.
* tuya sdk will callback the picture data in IPC_AI_FACE_DOWNLOAD_PIC_CB which is set in 
  tuya_ipc_ai_face_detect_storage_init circularly
* this func is blocked until all the picture done callback to user
**********************************************************************************/
OPERATE_RET tuya_ipc_ai_face_update_cloud_face();


#ifdef __cplusplus
}
#endif

#endif

