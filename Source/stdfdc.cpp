// Copyright (C) 2000-2016 Hideki EIRAKU
// SPDX-License-Identifier: GPL-2.0-or-later

#include "stdfdc.h"

import <cstdlib>;
import <cstring>;
import <fstream>;
import <iomanip>;
import <sstream>;

stdfdc::stdfdc (jvideo& d) : jfdc (d)
{
  int i;

  for (i = 0; i < 4; i++)
    fdd[i].insert = false;
}

void stdfdc::read()
{
  std::ifstream f;
  bool notready;
  bool sectornotfound;

  notready = false;
  sectornotfound = false;
  if (!fdd[drive & 3].insert)
    notready = true;
  if (!notready)
  {
    switch (bytes_per_sector)
    {
    case 2:
      datasize = 512 * (eot - sector + 1);
      if (datasize < 0)
        datasize = 0;
      if ((size_t)datasize > sizeof (data))
        datasize = sizeof (data);
      f.open (fdd[drive & 3].filename, std::ios::in | std::ios::binary);
      if (!f)
      {
        notready = true;
        break;
      }
      f.seekg ((cylinder * 18 + head * 9 + sector - 1) * 512);
      f.read ((char*)data, datasize);
      f.close();
      if (!f.good())
      {
        sectornotfound = true;
        break;
      }
      datai = 0;
      break;
    case 3:
      datasize = 1024 * (eot - sector + 1);
      if (datasize < 0)
        datasize = 0;
      if ((size_t)datasize > sizeof (data))
        datasize = sizeof (data);
      {
        std::ostringstream s;

        s << fdd[drive & 3].filename << '.' << std::setw (3) << std::setfill ('0')
          << (cylinder * 2 + head);
        f.open (s.str().c_str(), std::ios::in | std::ios::binary);
        if (!f)
        {
          f.open (fdd[drive & 3].filename, std::ios::in | std::ios::binary);
          if (!f)
            notready = true;
          else
          {
            sectornotfound = true;
            f.close();
          }
          break;
        }
        f.seekg ((sector - 1) * 1024);
        f.read ((char*)data, datasize);
        f.close();
        if (!f.good())
        {
          sectornotfound = true;
          break;
        }
      }
      datai = 0;
      break;
    default:
      f.open (fdd[drive & 3].filename, std::ios::in | std::ios::binary);
      if (!f)
        notready = true;
      else
      {
        sectornotfound = true;
        f.close();
      }
    }
  }
  if (notready)
  {
    st[0] = 0x40;
    st[1] = 0;
    st[3] = cylinder;
    st[5] = sector;
    transfertimeout();
    return;
  }
  if (sectornotfound)
  {
    st[0] = 0x40;
    st[1] = 4;
    st[3] = cylinder;
    st[5] = sector;
    transfertimeout();
    return;
  }
}

void stdfdc::postread()
{
  st[0] = 0;
  st[1] = 0x80;
  st[3] = cylinder;
  st[5] = eot;
}

void stdfdc::preformat()
{
  std::fstream f;
  bool notready;
  bool writeprotect;
  bool sectornotfound;

  notready = false;
  writeprotect = false;
  sectornotfound = false;
  if (!fdd[drive & 3].insert)
    notready = true;
  if (!notready)
  {
    switch (bytes_per_sector)
    {
    case 2:
      datasize = 4 * sectors_per_track;
      if (datasize < 0)
        datasize = 0;
      if ((size_t)datasize > sizeof (data))
        datasize = sizeof (data);
      f.open (fdd[drive & 3].filename, std::ios::in | std::ios::binary);
      f.close();
      if (!f.good())
      {
        notready = true;
        break;
      }
      f.open (fdd[drive & 3].filename, std::ios::in | std::ios ::out | std::ios::binary);
      f.close();
      if (!f.good())
      {
        writeprotect = true;
        break;
      }
      datai = 0;
      break;
    case 3:
      datasize = 4 * sectors_per_track;
      if (datasize < 0)
        datasize = 0;
      if ((size_t)datasize > sizeof (data))
        datasize = sizeof (data);
      if (0)
      {
        std::ostringstream s;

        s << fdd[drive & 3].filename << '.' << std::setw (3) << std::setfill ('0')
          << (cylinder * 2 + head);
        f.open (s.str().c_str(), std::ios::in | std::ios::out | std::ios::binary);
        f.close();
        if (!f.good())
        {
          f.open (fdd[drive & 3].filename, std::ios::in | std::ios::binary);
          f.close();
          if (!f.good())
            notready = true;
          else
            writeprotect = true;
          break;
        }
      }
      datai = 0;
      break;
    default:
      f.open (fdd[drive & 3].filename, std::ios::in | std::ios::binary);
      if (!f)
        notready = true;
      else
      {
        sectornotfound = true;
        f.close();
      }
    }
  }
  if (notready)
  {
    st[0] = 0x40;
    st[1] = 0;
    st[3] = cylinder;
    st[5] = sector;
    transfertimeout();
    return;
  }
  if (sectornotfound)
  {
    st[0] = 0x40;
    st[1] = 4;
    st[3] = cylinder;
    st[5] = sector;
    transfertimeout();
    return;
  }
  if (writeprotect)
  {
    st[0] = 0x40;
    st[1] = 2;
    st[3] = cylinder;
    st[5] = sector;
    transfertimeout();
    return;
  }
}

void stdfdc::format()
{
  std::fstream f;
  bool notready;
  bool writeprotect;
  bool sectornotfound;
  bool fdcerror;

  notready = false;
  writeprotect = false;
  sectornotfound = false;
  fdcerror = false;
  if (!fdd[drive & 3].insert)
    notready = true;
  if (!notready)
  {
    switch (bytes_per_sector)
    {
    case 2:
      f.open (fdd[drive & 3].filename, std::ios::in | std::ios::binary);
      f.close();
      if (!f.good())
      {
        notready = true;
        break;
      }
      f.open (fdd[drive & 3].filename, std::ios::in | std::ios ::out | std::ios::binary);
      if (!f.good())
      {
        writeprotect = true;
        break;
      }
      {
        unsigned int i;
        char buf[512];

        memset (buf, filler, sizeof (buf));
        for (i = 0; i < sectors_per_track * 4; i += 4)
        {
          f.seekp ((data[i] * 18 + data[i + 1] * 9 + data[i + 2] - 1) * 512);
          f.write (buf, 512);
        }
      }
      f.close();
      if (!f.good())
        fdcerror = true;
      break;
    case 3:
    {
      std::ostringstream s;
      unsigned int i;
      char buf[1024];

      for (i = 0; i < sectors_per_track * 4; i += 4)
      {
        memset (buf, filler, sizeof (buf));
        s << fdd[drive & 3].filename << '.' << std::setw (3) << std::setfill ('0')
          << (data[i] * 2 + data[i + 1]);
        f.open (s.str().c_str(), std::ios::in | std::ios::binary);
        f.close();
        if (!f.good())
        {
          f.open (fdd[drive & 3].filename, std::ios::in | std::ios::binary);
          f.close();
          if (!f.good())
            notready = true;
          else
            sectornotfound = true;
          break;
        }
        f.open (s.str().c_str(), std::ios::in | std::ios ::out | std::ios::binary);
        if (!f.good())
        {
          writeprotect = true;
          break;
        }
        f.seekp ((data[i + 2] - 1) * 1024);
        f.write (buf, 1024);
        f.close();
        if (!f.good())
        {
          fdcerror = true;
          break;
        }
      }
    }
    break;
    default:
      f.open (fdd[drive & 3].filename, std::ios::in | std::ios::binary);
      if (!f)
        notready = true;
      else
      {
        sectornotfound = true;
        f.close();
      }
    }
  }
  if (notready)
  {
    st[0] = 0x40;
    st[1] = 0;
    st[3] = cylinder;
    st[5] = sector;
    return;
  }
  if (sectornotfound)
  {
    st[0] = 0x40;
    st[1] = 4;
    st[3] = cylinder;
    st[5] = sector;
    return;
  }
  if (writeprotect)
  {
    st[0] = 0x40;
    st[1] = 2;
    st[3] = cylinder;
    st[5] = sector;
    return;
  }
  if (fdcerror)
  {
    st[0] = 0x80;
    st[1] = 0;
    st[3] = cylinder;
    st[5] = eot;
    return;
  }
  st[0] = 0;
  st[1] = 0x80;
  st[3] = cylinder;
  st[5] = eot;
}

void stdfdc::prewrite()
{
  std::fstream f;
  bool notready;
  bool writeprotect;
  bool sectornotfound;

  notready = false;
  writeprotect = false;
  sectornotfound = false;
  if (!fdd[drive & 3].insert)
    notready = true;
  if (!notready)
  {
    switch (bytes_per_sector)
    {
    case 2:
      datasize = 512 * (eot - sector + 1);
      if (datasize < 0)
        datasize = 0;
      if ((size_t)datasize > sizeof (data))
        datasize = sizeof (data);
      f.open (fdd[drive & 3].filename, std::ios::in | std::ios::binary);
      f.close();
      if (!f.good())
      {
        notready = true;
        break;
      }
      f.open (fdd[drive & 3].filename, std::ios::in | std::ios ::out | std::ios::binary);
      f.close();
      if (!f.good())
      {
        writeprotect = true;
        break;
      }
      datai = 0;
      break;
    case 3:
      datasize = 1024 * (eot - sector + 1);
      if (datasize < 0)
        datasize = 0;
      if ((size_t)datasize > sizeof (data))
        datasize = sizeof (data);
      {
        std::ostringstream s;

        s << fdd[drive & 3].filename << '.' << std::setw (3) << std::setfill ('0')
          << (cylinder * 2 + head);
        f.open (s.str().c_str(), std::ios::in | std::ios::binary);
        f.close();
        if (!f.good())
        {
          f.open (fdd[drive & 3].filename, std::ios::in | std::ios::binary);
          f.close();
          if (!f.good())
            notready = true;
          else
            sectornotfound = true;
          break;
        }
        f.open (s.str().c_str(), std::ios::in | std::ios ::out | std::ios::binary);
        f.close();
        if (!f.good())
        {
          writeprotect = true;
          break;
        }
      }
      datai = 0;
      break;
    default:
      f.open (fdd[drive & 3].filename, std::ios::in | std::ios::binary);
      if (!f)
        notready = true;
      else
      {
        sectornotfound = true;
        f.close();
      }
    }
  }
  if (notready)
  {
    st[0] = 0x40;
    st[1] = 0;
    st[3] = cylinder;
    st[5] = sector;
    transfertimeout();
    return;
  }
  if (sectornotfound)
  {
    st[0] = 0x40;
    st[1] = 4;
    st[3] = cylinder;
    st[5] = sector;
    transfertimeout();
    return;
  }
  if (writeprotect)
  {
    st[0] = 0x40;
    st[1] = 2;
    st[3] = cylinder;
    st[5] = sector;
    transfertimeout();
    return;
  }
}

void stdfdc::write()
{
  std::fstream f;
  bool notready;
  bool writeprotect;
  bool sectornotfound;
  bool fdcerror;

  notready = false;
  writeprotect = false;
  sectornotfound = false;
  fdcerror = false;
  if (!fdd[drive & 3].insert)
    notready = true;
  if (!notready)
  {
    switch (bytes_per_sector)
    {
    case 2:
      f.open (fdd[drive & 3].filename, std::ios::in | std::ios::binary);
      f.close();
      if (!f.good())
      {
        notready = true;
        break;
      }
      f.open (fdd[drive & 3].filename, std::ios::in | std::ios ::out | std::ios::binary);
      if (!f.good())
      {
        writeprotect = true;
        break;
      }
      f.seekp ((cylinder * 18 + head * 9 + sector - 1) * 512);
      f.write ((char*)data, datasize);
      f.close();
      if (!f.good())
        fdcerror = true;
      break;
    case 3:
    {
      std::ostringstream s;

      s << fdd[drive & 3].filename << '.' << std::setw (3) << std::setfill ('0')
        << (cylinder * 2 + head);
      f.open (s.str().c_str(), std::ios::in | std::ios::binary);
      f.close();
      if (!f.good())
      {
        f.open (fdd[drive & 3].filename, std::ios::in | std::ios::binary);
        f.close();
        if (!f.good())
          notready = true;
        else
          sectornotfound = true;
        break;
      }
      f.open (s.str().c_str(), std::ios::in | std::ios ::out | std::ios::binary);
      if (!f.good())
      {
        writeprotect = true;
        break;
      }
      f.seekp ((sector - 1) * 1024);
      f.write ((char*)data, datasize);
      f.close();
      if (!f.good())
        fdcerror = true;
    }
    break;
    default:
      f.open (fdd[drive & 3].filename, std::ios::in | std::ios::binary);
      if (!f)
        notready = true;
      else
      {
        sectornotfound = true;
        f.close();
      }
    }
  }
  if (notready)
  {
    st[0] = 0x40;
    st[1] = 0;
    st[3] = cylinder;
    st[5] = sector;
    return;
  }
  if (sectornotfound)
  {
    st[0] = 0x40;
    st[1] = 4;
    st[3] = cylinder;
    st[5] = sector;
    return;
  }
  if (writeprotect)
  {
    st[0] = 0x40;
    st[1] = 2;
    st[3] = cylinder;
    st[5] = sector;
    return;
  }
  if (fdcerror)
  {
    st[0] = 0x80;
    st[1] = 0;
    st[3] = cylinder;
    st[5] = eot;
    return;
  }
  st[0] = 0;
  st[1] = 0x80;
  st[3] = cylinder;
  st[5] = eot;
}

void stdfdc::recalibrate()
{
  st[0] = 0x20;
}

void stdfdc::seek()
{
  st[0] = 0x20;
}

void stdfdc::insert (t16 drive, char* filename)
{
  eject (drive);
  fdd[drive & 3].filename = new char[strlen (filename) + 1];
  strcpy (fdd[drive & 3].filename, filename);
  if (fdd[drive & 3].filename)
    fdd[drive & 3].insert = true;
}

void stdfdc::eject (t16 drive)
{
  if (fdd[drive & 3].insert)
    delete[] fdd[drive & 3].filename;
  fdd[drive & 3].insert = false;
}
