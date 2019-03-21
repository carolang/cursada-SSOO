#include "srv.h"

// Devuelve TRUE sii el otro tiene mas prioridad que yo
int tieneMasPrioridad(int otro_prio, int otro_id, int mi_prio, int mi_id){
	printf("Comparo (%d,%d) con (%d,%d)\n", otro_prio, otro_id, mi_prio, mi_id);
	if(otro_prio < mi_prio) return TRUE;
	if(mi_prio < otro_prio) return FALSE;
	
	if(otro_id < mi_id) return TRUE;
	return FALSE;
}

void servidor(int mi_cliente)
{
    MPI_Status status; int origen, tag;
    int hay_pedido_local = FALSE;
    int listo_para_salir = FALSE;
    int otorgue_acceso = FALSE;
    
    // Variables auxiliares
    int nodos;
    int mi_id;
    MPI_Comm_size(COMM_WORLD, &nodos);		// Cambiar por la cantidad de nodos
    MPI_Comm_rank(COMM_WORLD, &mi_id);		// Cambiar por la cantidad de nodos
    nodos /=2;
    
    int mi_prioridad;
    int numero_mas_alto = 0;
    
    // Para atender los REQUEST que llegan
    int respuestas_a_recibir = 0;
    int respuesta_a_enviar[nodos];
    int i;
    for(i=0; i<nodos; i++){
		respuesta_a_enviar[i] = FALSE;
	}
	
	// Protocolo de finalizacion
	int server_vivo[nodos];
    for(i=0; i<nodos; i++){
		server_vivo[i] = TRUE;
	}
	int cant_otros_servers_vivos = nodos-1;			// Es la cantidad de servers vivos distintos a mi
    int cant_ack_esperados = cant_otros_servers_vivos;
    
    // Buffers para el programa
    int buf_rec;
    
      
    while( !listo_para_salir || cant_ack_esperados > 0) {
        
        MPI_Recv(&buf_rec, 1, MPI_INT, ANY_SOURCE, ANY_TAG, COMM_WORLD, &status);
        origen = status.MPI_SOURCE;
        tag = status.MPI_TAG;
        
        if (tag == TAG_PEDIDO_CLI) {
            assert(origen == mi_cliente);
            debug("Mi cliente solicita acceso exclusivo");
            assert(hay_pedido_local == FALSE);
            hay_pedido_local = TRUE;
            respuestas_a_recibir = cant_otros_servers_vivos;
            
            // Le mando un request a todos los servers vivos
            mi_prioridad = numero_mas_alto + 1;
            int i;
            for(i=0; i<nodos; i++){
				int srv = 2*i;
				if(srv == mi_id || !server_vivo[i]) continue;
				debug("Mando request a otro server");
				MPI_Send(&mi_prioridad, 1, MPI_INT, srv, TAG_PEDIDO_SRV, COMM_WORLD);
			}
        }
        
        else if (tag == TAG_LIBERO) {
            assert(origen == mi_cliente);
            debug("Mi cliente libera su acceso exclusivo");
            assert(hay_pedido_local == TRUE);
            hay_pedido_local = FALSE;
            otorgue_acceso = FALSE;
            
            // Avisar a los procesos de menor prioridad que termino
            int i;
            for(i=0; i<nodos; i++){
				int srv = 2*i;
				if(srv==mi_id || !server_vivo[i]) continue;
				
				// Mando un READY al server solamente si 
				if(!respuesta_a_enviar[i]) continue;
				MPI_Send(NULL, 0, MPI_INT, srv, TAG_HABILITAR, COMM_WORLD);
				respuesta_a_enviar[i] = FALSE;
			}				
        }
        
        else if (tag == TAG_TERMINE) {
            assert(origen == mi_cliente);
            debug("Mi cliente avisa que terminó");
            listo_para_salir = TRUE;
            
            // Aviso al resto de los servidores que quiero terminar
            for(i=0; i<nodos; i++){
				int srv = 2*i;
				if(srv == mi_id || !server_vivo[i]) continue;
				debug("Mando pedido de finalizar a otro server");
				MPI_Send(&mi_id, 1, MPI_INT, srv, TAG_FINALIZAR, COMM_WORLD);
			}
			cant_ack_esperados = cant_otros_servers_vivos;
        }
        
        else if (tag == TAG_PEDIDO_SRV){
			// Otro servidor pidio acceso para su cliente
			numero_mas_alto = (numero_mas_alto > buf_rec ? numero_mas_alto : buf_rec);
			
			// Si el pedido es mas prioritario que yo le doy el OK, si no encolo darle el OK
			if(!hay_pedido_local || tieneMasPrioridad(buf_rec, status.MPI_SOURCE, mi_prioridad, mi_id)){
				debug("Me llego un mensaje MAS prioritario que el mio");
				MPI_Send(NULL, 0, MPI_INT, status.MPI_SOURCE, TAG_HABILITAR, COMM_WORLD);
			}
			else{
				debug("Me llego un mensaje MENOS prioritario que el mio");
				respuesta_a_enviar[status.MPI_SOURCE / 2] = TRUE;
			}
		}
		
		else if (tag == TAG_HABILITAR){
			// Me llego una habilitacion de otro servidor
			respuestas_a_recibir--;
		}
		
		else if (tag == TAG_ACK_FINALIZAR){
			// Alguien me esta autorizando a terminar
			cant_ack_esperados--;
		}
		
		else if (tag == TAG_FINALIZAR){
			// Si un server me dice que quiere terminar, lo mato y le mando el ACK
			cant_otros_servers_vivos--;
			server_vivo[status.MPI_SOURCE / 2] = FALSE;
			MPI_Send(NULL, 0, MPI_INT, status.MPI_SOURCE, TAG_ACK_FINALIZAR, COMM_WORLD);
		}
		
		if(respuestas_a_recibir == 0 && hay_pedido_local && !otorgue_acceso){
			// Si hay pedido local y ya me contestaron todos, doy acceso
			// Esto esta afuera del caso TAG_REPLY por si soy el único nodo.
			MPI_Send(NULL, 0, MPI_INT, mi_cliente, TAG_OTORGADO, COMM_WORLD);
			otorgue_acceso = TRUE;
		}
    }
    
}

