void *actionHandler(void *arg);
void *receiveFunc(void *arg);
void *receiveClient(void *arg);

typedef struct {
    uint8_t FrameType;    //0x10
    uint16_t MsgLen;      //0x08 + user + ClientId
    uint16_t UserLen;
    char User [20];
    uint8_t Version;      //0x04 protocol
    uint8_t ConnectFlags; //0x02 clean session
    uint16_t KeepAlive;
    uint16_t ClientIdLen;
    char ClientId [100];
} Connect; 

typedef struct {
    uint8_t FrameType; //0x20
    uint8_t MsgLen;    //0x02
    uint8_t Flags;     //0x00
    uint8_t ReturnStatus;
    //0x00 Connection Accepted
    //0x01 Client Limit Reached
    //0x02 Malformed Frame
} ConnAck;

typedef struct {
    uint8_t FrameType; //0x30
    uint8_t FrameLen; //0x04 + topic + msg
    uint16_t TopicLen;
    char Topic [20];
    uint16_t MsgLen;
    char Msg [100];
} Publish;

typedef struct {
    uint8_t FrameType; //0x40
} PubAck;

typedef struct {
    uint8_t FrameType; //0x50
    uint8_t FrameLen; //0x03 + topic
    uint16_t TopicLen;
    char Topic [20];
} Subscribe;

typedef struct {
    uint8_t FrameType; //0x60
    uint8_t FrameLen; //0x03
    uint16_t PacketId;
    uint8_t returnCode;
} SubAck;

typedef struct {
    uint8_t FrameType; //0x70
} PubRec;

typedef struct {
    uint8_t FrameType; //0x80
} PubRel;

typedef struct {
    uint8_t FrameType; //0x90
} PubComp;

typedef struct {
    uint8_t FrameType; //0xA0
    uint8_t FrameLen;
    uint16_t PacketId;
    uint16_t TopicLen;
    char Topic [20];
    uint8_t QoS;
} Unsubscribe;

typedef struct {
    uint8_t FrameType; //0xB0
    uint8_t FrameLen;
    uint16_t PacketId;
} UnsubAck;

typedef struct {
    uint8_t FrameType; //Req=0xC0 Res=0xD0
    uint8_t MsgLen;      //0x00
}Pings;

typedef struct{
    uint8_t FrameType; //0xF0
} Disconnect;