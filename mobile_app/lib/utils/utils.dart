import 'package:flutter/foundation.dart';

void logger(dynamic object) {
  if (kDebugMode) {
    print(object);
  }
}
