

uint16_t hex_to_int(uint8_t c);
uint16_t hex_to_ascii(uint16_t c);
void UART0_send_byte(uint8_t byte);
void UART0_send(unsigned char *BufferPtr, unsigned short Length );
void process_command(char *cmd);
double atof(const char *s);
