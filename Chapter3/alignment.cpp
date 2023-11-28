#include <cstdio>
#include <cstdint>
#include <cstddef>

struct PoorlyAlignedData {
  char c;
  uint16_t u;
  double d;
  int16_t i;
};

struct WellAlignedData {
  double d;
  uint16_t u;
  int16_t i;
  char c;
};

#pragma pack(push, 1) // pack the struct tightly
struct PackedData {
  double d;
  uint16_t u;
  int16_t i;
  char c;
};
#pragma pack(pop)

#pragma pack(push, 2) // compiler will align each member on a 2-byte boundary.
struct PackedData2 {
  double d;
  uint16_t u;
  int16_t i;
  char c;
};
#pragma pack(pop)

int main() {
  printf("PoorlyAlignedData c:%lu u:%lu d:%lu i:%lu size:%lu\n",
         offsetof(struct PoorlyAlignedData,c), offsetof(struct PoorlyAlignedData,u), offsetof(struct PoorlyAlignedData,d), offsetof(struct PoorlyAlignedData,i), sizeof(PoorlyAlignedData));
  // the rule is align before a member, and paddle to make the total storage size a multiple of the largest member.
  printf("WellAlignedData d:%lu u:%lu i:%lu c:%lu size:%lu\n",
         offsetof(struct WellAlignedData,d), offsetof(struct WellAlignedData,u), offsetof(struct WellAlignedData,i), offsetof(struct WellAlignedData,c), sizeof(WellAlignedData));
  printf("PackedData d:%lu u:%lu i:%lu c:%lu size:%lu\n",
         offsetof(struct PackedData,d), offsetof(struct PackedData,u), offsetof(struct PackedData,i), offsetof(struct PackedData,c), sizeof(PackedData));
  printf("PackedData2 d:%lu u:%lu i:%lu c:%lu size:%lu\n",
          offsetof(struct PackedData2,d), offsetof(struct PackedData2,u), offsetof(struct PackedData2,i), offsetof(struct PackedData2,c), sizeof(PackedData2));

}