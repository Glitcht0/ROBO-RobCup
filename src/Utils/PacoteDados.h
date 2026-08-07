#pragma once
#include <cstdint>

#pragma pack(push, 1)
struct PacoteDados {
    uint32_t id_mensagem;    
    float sliders[4];        
    bool powerAtivo;         
};
#pragma pack(pop)