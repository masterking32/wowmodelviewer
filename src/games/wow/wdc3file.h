#ifndef WDC3FILE_H
#define WDC3FILE_H

#include "types.h"
#include "wdb5file.h"

class WDC3File : public WDB5File
{
public:

  struct header
  {
    char   magic[4];               // 'WDC3'
    uint32 record_count;           // this is for all sections combined now
    uint32 field_count;
    uint32 record_size;
    uint32 string_table_size;      // this is for all sections combined now
    uint32 table_hash;             // hash of the table name
    uint32 layout_hash;            // this is a hash field that changes only when the structure of the data changes
    uint32 min_id;
    uint32 max_id;
    uint32 locale;                 // as seen in TextWowEnum
    uint16 flags;                  // possible values are listed in Known Flag Meanings
    uint16 id_index;               // this is the index of the field containing ID values; this is ignored if flags & 0x04 != 0
    uint32 total_field_count;      // from WDC1 onwards, this value seems to always be the same as the 'field_count' value
    uint32 bitpacked_data_offset;  // relative position in record where bitpacked data begins; not important for parsing the file
    uint32 lookup_column_count;
    uint32 field_storage_info_size;
    uint32 common_data_size;
    uint32 pallet_data_size;
    uint32 section_count;          // new to WDC3, this is number of sections of data (records + copy table + id list + relationship map = a section)
  };

  struct section_header
  {
    uint64 tact_key_hash;          // TactKeyLookup hash
    uint32 file_offset;            // Absolute position to the beginning of the section
    uint32 record_count;           // 'record_count' for the section
    uint32 string_table_size;      // 'string_table_size' for the section
    uint32 offset_records_end;     // Offset to the spot where the records end in a file with an offset map structure;
    uint32 id_list_size;           // Size of the list of ids present in the section
    uint32 relationship_data_size; // Size of the relationship data in the section
    uint32 offset_map_id_count;    // Count of ids present in the offset map in the section
    uint32 copy_table_count;       // Count of the number of deduplication entries (you can multiply by 8 to mimic the old 'copy_table_size' field
  };


  explicit WDC3File(const QString & file);
  ~WDC3File();

  bool open();

  bool close();

  std::vector<std::string> get(unsigned int recordIndex, const core::TableStructure * structure) const;

private:
  enum FIELD_COMPRESSION
  {
    // None -- the field is a 8-, 16-, 32-, or 64-bit integer in the record data
    NONE,
    // Bitpacked -- the field is a bitpacked integer in the record data.  It
    // is field_size_bits long and starts at field_offset_bits.
    // A bitpacked value occupies
    //   (field_size_bits + (field_offset_bits & 7) + 7) / 8
    // bytes starting at byte
    //   field_offset_bits / 8
    // in the record data.  These bytes should be read as a little-endian value,
    // then the value is shifted to the right by (field_offset_bits & 7) and
    // masked with ((1ull << field_size_bits) - 1).
    BITPACKED,
    // Common data -- the field is assumed to be a default value, and exceptions
    // from that default value are stored in the corresponding section in
    // common_data as pairs of { uint32_t record_id; uint32_t value; }.
    COMMON_DATA,
    // Bitpacked indexed -- the field has a bitpacked index in the record data.
    // This index is used as an index into the corresponding section in
    // pallet_data.  The pallet_data section is an array of uint32_t, so the index
    // should be multiplied by 4 to obtain a byte offset.
    BITPACKED_INDEXED,
    // Bitpacked indexed array -- the field has a bitpacked index in the record
    // data.  This index is used as an index into the corresponding section in
    // pallet_data.  The pallet_data section is an array of uint32_t[array_count],
    //
    BITPACKED_INDEXED_ARRAY,
    // Same as field_compression_bitpacked
    BITPACKED_SIGNED
  };

  struct field_storage_info
  {
    uint16 field_offset_bits;
    uint16 field_size_bits; // very important for reading bitpacked fields; size is the sum of all array pieces in bits - for example, uint32[3] will appear here as '96'
    // additional_data_size is the size in bytes of the corresponding section in
    // common_data or pallet_data.  These sections are in the same order as the
    // field_info, so to find the offset, add up the additional_data_size of any
    // previous fields which are stored in the same block (common_data or
    // pallet_data).
    uint32 additional_data_size;
    uint32 storage_type;
    uint32 val1;
    uint32 val2;
    uint32 val3;
  };

  void readWDC3Header();

  bool readFieldValue(unsigned int recordIndex, unsigned int fieldIndex, uint arrayIndex, uint arraySize, unsigned int & result) const;
  uint32 readBitpackedValue(field_storage_info info, unsigned char * recordOffset) const;
  int32 readSignedBitpackedValue(field_storage_info info, unsigned char * recordOffset) const;

  header m_header;
  section_header * m_sectionHeader;
  std::vector<field_storage_info> m_fieldStorageInfo;

  std::map<uint32, uint32> m_palletBlockOffsets;
  std::map<uint32, std::map<uint32, uint32> > m_commonData;
  std::map<uint32, std::string> m_relationShipData;

  unsigned char * m_sectionData;
  unsigned char * m_palletData;
};

#endif
