#include "seam_carver.hpp"

// implement the rest of SeamCarver's functions here

// given functions below, DO NOT MODIFY

SeamCarver::SeamCarver(const ImagePPM& image): image_(image) {
  height_ = image.GetHeight();
  width_ = image.GetWidth();
}

void SeamCarver::SetImage(const ImagePPM& image) {
  image_ = image;
  width_ = image.GetWidth();
  height_ = image.GetHeight();
}

const ImagePPM& SeamCarver::GetImage() const {
  return image_;
}

int SeamCarver::GetHeight() const {
  return height_;
}

int SeamCarver::GetWidth() const {
  return width_;
}

int SeamCarver::GetEnergy(int row, int col) const {
  int col1 = col - 1;
  int col2 = col + 1;
  int row1 = row - 1;
  int row2 = row + 1;
  if (col1 == -1) {
    col1 = width_ - 1;
  }
  if (col2 == width_) {
    col2 = 0;
  }
  if (row1 == -1) {
    row1 = height_ - 1;
  }
  if (row2 == height_) {
    row2 = 0;
  }
  int rcol = image_.GetPixel(row, col1).GetRed() - image_.GetPixel(row, col2).GetRed();
  int gcol = image_.GetPixel(row, col1).GetGreen() - image_.GetPixel(row, col2).GetGreen();
  int bcol = image_.GetPixel(row, col1).GetBlue() - image_.GetPixel(row, col2).GetBlue();
  int rrow = image_.GetPixel(row1, col).GetRed() - image_.GetPixel(row2, col).GetRed();
  int grow = image_.GetPixel(row1, col).GetGreen() - image_.GetPixel(row2, col).GetGreen();
  int brow = image_.GetPixel(row1, col).GetBlue() - image_.GetPixel(row2, col).GetBlue();
  int delta_colsq = rcol * rcol + gcol * gcol + bcol * bcol;
  int delta_rowsq = rrow * rrow + grow * grow + brow * brow;
  return delta_colsq + delta_rowsq;
}

int* SeamCarver::GetHorizontalSeam() const {
  int* ret = new int[width_];
  int** values = GetValuesHor();
  ret[0] = GetRetHor(values);
  int counter = GetRetHor(values);
  for (int col = 1; col < width_; ++col) {
    int temp = values[counter][col];
    ret[col] = ret[col - 1];
    if (ret[col - 1] == 0) {
      if (values[counter + 1][col] < temp) {
        counter++;
      }
    } else if (counter == height_ - 1) {
      if (values[counter - 1][col] < temp) {
        counter--;
      }
    } else {
      if (values[counter - 1][col] < temp) {
        if (values[counter - 1][col] <= values[counter + 1][col]) {
          counter--;
        }
      } else if (values[counter + 1][col] < temp) {
        if (values[counter + 1][col] < values[counter - 1][col]) {
          counter++;
        }
      }
    }
    ret[col] = counter;
  }
  for (int i = 0; i < height_; ++i) {
    delete[] values[i];
  }
  delete[] values;
  values = nullptr;
  return ret;
}

int* SeamCarver::GetVerticalSeam() const {
  int* ret = new int[height_];
  int** values = GetValuesVert();
  ret[0] = GetRetVert(values);
  int counter = GetRetVert(values);
  for (int row = 1; row < height_; ++row) {
    int temp = values[row][counter];
    ret[row] = ret[row - 1];
    if (ret[row - 1] == 0) {
      if (values[row][counter + 1] < temp) {
        counter++;
      }
    } else if (counter == width_ - 1) {
      if (values[row][counter - 1] < temp) {
        counter--;
      }
    } else {
      if (values[row][counter - 1] < temp) {
        if (values[row][counter - 1] <= values[row][counter + 1]) {
          counter--;
        }
      } else if (values[row][counter + 1] < temp) {
        if (values[row][counter + 1] < values[row][counter - 1]) {
          counter++;
        }
      }
    }
    ret[row] = counter;
  }
  for (int i = 0; i < height_; ++i) {
    delete[] values[i];
  }
  delete[] values;
  values = nullptr;
  return ret;
}

void SeamCarver::RemoveHorizontalSeam() {
  int* seam = GetHorizontalSeam();
  image_.RemoveHorizontalSeam(seam);
  delete[] seam;
  seam = nullptr;
  height_ -= 1;
}

void SeamCarver::RemoveVerticalSeam() {
  int* seam = GetVerticalSeam();
  image_.RemoveVerticalSeam(seam);
  delete[] seam;
  seam = nullptr;
  width_ -= 1;
}

int** SeamCarver::Empty2dArray() const {
  int** arr = new int*[height_];
  for (int i = 0; i < height_; ++i) {
    arr[i] = new int[width_];
  }
  return arr;
}

int** SeamCarver::GetValuesVert() const {
  int** values = Empty2dArray();
  int best = 0;
  for (int col = 0; col < width_; ++col) {
    values[height_ - 1][col] = GetEnergy(height_ - 1, col);
  }
  for (int row = height_ - 2; row >= 0; --row) {
    for (int col = 0; col < width_; ++col) {
      if (col == 0) {
        best = GetMin(values[row + 1][col], values[row + 1][col + 1]);
      } else if (col == width_ - 1) {
        best = GetMin(values[row + 1][col - 1], values[row + 1][col]);
      } else {
        best = GetMin(values[row + 1][col - 1], values[row + 1][col], values[row + 1][col + 1]);
      }
      values[row][col] = best + GetEnergy(row, col);
    }
  }
  return values;
}

int** SeamCarver::GetValuesHor() const {
  int** values = Empty2dArray();
  int best = 0;
  for (int row = 0; row < height_; ++row) {
    values[row][width_ - 1] = GetEnergy(row, width_ - 1);
  }
  for (int col = width_ - 2; col >= 0; --col) {
    for (int row = 0; row < height_; ++row) {
      if (row - 1 < 0) {
        best = GetMin(values[row][col + 1], values[row + 1][col + 1]);
      } else if (row == height_ - 1) {
        best = GetMin(values[row - 1][col + 1], values[row][col + 1]);
      } else {
        best = GetMin(values[row - 1][col + 1], values[row][col + 1], values[row + 1][col + 1]);
      }
      values[row][col] = best + GetEnergy(row, col);
    }
  }
  return values;
}

int SeamCarver::GetRetVert(int** values) const {
  int ret = 0;
  int min = values[0][0];
  for (int col = 0; col < width_; ++col) {
    if (values[0][col] < min) {
      min = values[0][col];
      ret = col;
    }
  }
  return ret;
}

int SeamCarver::GetRetHor(int** values) const {
  int ret = 0;
  int min = values[0][0];
  for (int row = 0; row < height_; ++row) {
    if (values[row][0] < min) {
      min = values[row][0];
      ret = row;
    }
  }
  return ret;
}

int GetMin(int num1, int num2, int num3) {
  if (num1 < num2 && num1 < num3) {
    return num1;
  } else if (num3 < num2 && num3 < num1) {
    return num3;
  } else {
    return num2;
  }
}

int GetMin(int num1, int num2) {
  if (num2 < num1) {
    return num2;
  } 
  return num1;
}

