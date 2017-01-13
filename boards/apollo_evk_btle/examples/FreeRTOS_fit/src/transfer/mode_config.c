#include "mode_config.h"
#include <string.h>
#include "transfer_profile.h"
#include "ble_config.h"
#include "debug.h"

uint8_t mode_config_receive_process(transfer_profile_st *l_pack_st)
{
	uint8_t direction,*data,ack_cmd,ack_result;
	uint16_t pack_length;
	
	direction 	= (l_pack_st->pack_type >> 5)&0x07;	
	data 		= l_pack_st->data;
	pack_length = transfer_profile_com_get_value(l_pack_st->pack_length,2);
	
	ack_cmd 	= data[0];
	ack_result 	= data[1];
	
	if(direction == READ_DIREC)
	{
		if(ack_result == 0x00 && pack_length > 0x02)//ok
		{
			switch(ack_cmd)
			{	
				case CONFIG_ADV_CMD:
					break;
					
				case CONFIG_CONN_CMD:
					break;
					
				case CONFIG_NOTIFY_CMD:
					break;
					
				case CONFIG_MAC_CMD:
					break;
					
				case CONFIG_NAME_CMD:
					break;
					
				case CONFIG_DEVICE_ID_CMD:
					break;
					
				case CONFIG_DEVICE_TYPE_CMD:
					break;
					
				case CONFIG_APP_VERSION:
					break;
					
				case CONFIG_BLE_VERSION:
					break;
					
				case CONFIG_BLE_HARD_VERSION_CMD:
					break;
					
				case CONFIG_ALL_VERSION_CMD:
					break;
					
				case CONFIG_PRODUCTS_TYPE_CMD:
					break;
					
				case CONFIG_MANUFACT_NAME_CMD:
					break;
					
				case CONFIG_BLE_TYPE_CMD:
					break;

				default:
					break;
			}
		}	
	}
	else if(direction == WRITE_DIREC)
	{
		QPRINTF("ack cmd:%d, ack result:%d,config_init_flag:%x\r\n",ack_cmd,ack_result,g_mode_config.config_init_flag);
		if(ack_result == 0x00)//ok
		{
			g_mode_config.config_init_flag &= ~(1<<ack_cmd);
		}
	}

	/*如果写方向，即往对方配置信息，所以在得到应答后不需要再次给对方应答*/
	return 1;
}

uint8_t mode_config_loop_process(void)
{
	uint8_t direction;
	uint16_t data_length;
	transfer_profile_st l_pack_st;
	
	if(g_mode_config.config_init_flag == 0)
		return 0;

	for(uint8_t config =CONFIG_ADV_CMD;config<CONFIG_CMD_MAX;config++)
	{
		if(g_mode_config.config_init_flag & (1<<config))
		{
			direction = g_mode_config.config_operation[config];
			transfer_profile_get_packet_no(l_pack_st.pack_no);
			l_pack_st.frame_no = 0x00;

			l_pack_st.pack_type = 0x00;
			l_pack_st.pack_type |= (direction<<OPERATION_SHIFT_BIT);
			l_pack_st.pack_type |= (PACK_CONFIG_TYPE<<DATA_TYPE_SHIFT_BIT);
			l_pack_st.pack_cmd = config;
			
			if(direction == WRITE_DIREC)
			{
				/*如果配置方向为写的话，就把相应的数据写到data里面去*/
				switch(config)
				{
					case CONFIG_ADV_CMD:
						data_length = sizeof(adv_parm_st);
						memcpy(l_pack_st.data,&(g_mode_config.adv_parm),data_length);
						break;

					case CONFIG_CONN_CMD:
						data_length = sizeof(con_parm_st);
						memcpy(l_pack_st.data,&(g_mode_config.con_parm),data_length);
						break;

					case CONFIG_NOTIFY_CMD:
						data_length = sizeof(notify_parm_st);
						memcpy(l_pack_st.data,&(g_mode_config.notify_parm),data_length);
						break;

					case CONFIG_MAC_CMD:
						data_length = sizeof(g_mode_config.ble_parm.mac_addr);
						memcpy(l_pack_st.data,&(g_mode_config.ble_parm.mac_addr),data_length);
						break;
						
					case CONFIG_NAME_CMD:
						data_length = strlen((const char*)(g_mode_config.ble_parm.ble_name));
						memcpy(l_pack_st.data,&(g_mode_config.ble_parm.ble_name),data_length);
						break;

					case CONFIG_DEVICE_ID_CMD:
						data_length = sizeof(g_mode_config.device_parm.id);
						memcpy(l_pack_st.data,&(g_mode_config.device_parm.id),data_length);
						break;

					case CONFIG_DEVICE_TYPE_CMD:
						data_length = sizeof(g_mode_config.device_parm.type);
						memcpy(l_pack_st.data,&(g_mode_config.device_parm.type),data_length);
						break;
						
					default:break;
				}
			}
			else if(direction == READ_DIREC)
			{
				/*如果配置方向为读的话，只需要进行读操作，不需要在data里面填充内容*/
				data_length = 0;
			}
			
			transfer_profile_com_set_value((uint32_t)data_length,l_pack_st.pack_length,2);
			transfer_profile_add_crc_transmit(&l_pack_st);
			break;
		}
	}

	if(g_mode_config.config_init_flag > 0)
		return 1;
	else
		return 0;
}


