#pragma once
/*
  Zipios++ - a small C++ library that provides easy access to .zip files.
  Copyright (C) 2000-2015  Thomas Sondergaard

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

/** \file
 * \brief Header file containing classes and functions for reading the central
 * directory and local header fields in a zip archive.
 */

#include "zipios++/fileentry.hpp"


namespace zipios
{


class ZipCDirEntry;

/** A concrete implementation of the abstract FileEntry base class for
 * ZipFile entries, specifically for representing the information
 * present in the local headers of file entries in a zip file.
 */
class ZipLocalEntry : public FileEntry
{
public:
    inline ZipLocalEntry(std::string const& filename = "", buffer_t const& extra_field = buffer_t())
        : FileEntry(filename)
        //, m_gp_bitfield(0) -- auto-init
    {
        setDefaultExtract();
        setExtra(extra_field);
    }

    inline ZipLocalEntry&       operator = (ZipLocalEntry const& src);
    virtual pointer_t           clone() const override;

    virtual                     ~ZipLocalEntry() override {}

    bool                        operator == (ZipCDirEntry const& ze) const;

    virtual size_t              getCompressedSize() const override;
    virtual buffer_t            getExtra() const override;
    virtual StorageMethod       getMethod() const override;

    virtual void                setCompressedSize(size_t size) override;
    virtual void                setCrc(crc32_t crc) override;
    void                        setDefaultExtract();
    virtual void                setExtra(buffer_t const& extra) override;
    virtual void                setMethod(StorageMethod method) override;
    virtual std::string         toString() const override;
    int                         getLocalHeaderSize() const;
    bool                        trailingDataDescriptor() const;

    void                        read(std::istream& is);
    void                        write(std::ostream& os);

protected:
    static uint32_t const       g_signature;

    // TODO: we need defaults for all those values!
    uint16_t                    m_extract_version;
    uint16_t                    m_gp_bitfield = 0;
    uint16_t                    m_compress_method;
    //uint16_t                    m_last_mod_ftime;
    //uint16_t                    m_last_mod_fdate;
    size_t                      m_compressed_size;
    uint16_t                    m_filename_len;
    uint16_t                    m_extra_field_len;

    buffer_t                    m_extra_field;
};


/** A struct containing fields for the entries in a zip file data
 descriptor, that trails the compressed data in files that were
 created by streaming, ie where the zip compressor cannot seek back
 to the local header and store the data. */
//struct DataDescriptor
//{
//    uint32_t    crc_32;
//    uint32_t    compressed_size;
//    uint32_t    uncompressed_size;
//};



/** Specialization of ZipLocalEntry, that add fields for storing the
    extra information, that is only present in the entries in the zip
    central directory and not in the local entry headers. */
class ZipCDirEntry : public ZipLocalEntry
{
public:
    typedef std::vector<ZipCDirEntry>   vector_t;

    inline ZipCDirEntry(std::string const& filename = "",
                        std::string const& file_comment = "",
                        std::vector< unsigned char > const& extra_field = std::vector<unsigned char>())
        : ZipLocalEntry(filename, extra_field)
        // TODO -- missing initialization of many member variables
        //, m_disk_num_start(0) -- auto-init
        //, m_intern_file_attr(0) -- auto-init
        //, m_extern_file_attr(0x81B40000) -- auto-init

        // FIXME: I do not understand the external mapping, simply
        //        copied value for a file with -rw-rw-r-- permissions
        //        compressed with info-zip
    {
        setComment(file_comment);
        setDefaultWriter();
    }

    virtual pointer_t           clone() const override;

    virtual                     ~ZipCDirEntry() override {}

    void                        setDefaultWriter();

    inline ZipCDirEntry&        operator = (ZipCDirEntry const& rhs);
    virtual std::string         toString() const override;

    virtual std::string         getComment() const override;
    virtual void                setComment(std::string const& comment) override;

    virtual uint32_t            getLocalHeaderOffset() const;
    virtual void                setLocalHeaderOffset(uint32_t offset);

    int                         getCDirHeaderSize() const;

    void                        read(std::istream& is);
    void                        write(std::ostream& os);

private:
    static const uint32_t       g_signature;

    uint16_t                    m_writer_version;
    uint16_t                    m_file_comment_len;
    uint16_t                    m_disk_num_start = 0;
    uint16_t                    m_intern_file_attr = 0;
    uint32_t                    m_extern_file_attr = 0x81B40000;
    uint32_t                    m_rel_offset_loc_head;
    std::string                 m_file_comment;
};




inline bool operator == (ZipCDirEntry const& ze, ZipLocalEntry const& zlh)
{
    return zlh == ze;
}


inline bool operator != (ZipLocalEntry const& zlh, ZipCDirEntry const& ze)
{
    return !(zlh == ze);
}


inline bool operator != (ZipCDirEntry const& ze, ZipLocalEntry const& zlh)
{
    return !(zlh == ze);
}

// Inline member functions

ZipCDirEntry& ZipCDirEntry::operator = (ZipCDirEntry const& src)
{
    m_writer_version      = src.m_writer_version      ;
    m_extract_version     = src.m_extract_version     ;
    m_gp_bitfield         = src.m_gp_bitfield         ;
    m_compress_method     = src.m_compress_method     ;
    //m_last_mod_ftime      = src.m_last_mod_ftime      ;
    //m_last_mod_fdate      = src.m_last_mod_fdate      ;
    m_crc_32              = src.m_crc_32              ;
    m_compressed_size     = src.m_compressed_size     ;
    m_uncompressed_size   = src.m_uncompressed_size   ;
    m_filename_len        = src.m_filename_len        ;
    m_extra_field_len     = src.m_extra_field_len     ;
    m_file_comment_len    = src.m_file_comment_len    ;
    m_disk_num_start      = src.m_disk_num_start      ;
    m_intern_file_attr    = src.m_intern_file_attr    ;
    m_extern_file_attr    = src.m_extern_file_attr    ;
    m_rel_offset_loc_head = src.m_rel_offset_loc_head ;

    m_filename            = src.m_filename            ;
    m_extra_field         = src.m_extra_field         ;
    m_file_comment        = src.m_file_comment        ;

    return *this;
}


} // zipios namespace
// vim: ts=4 sw=4 et