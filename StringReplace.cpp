std::string& StringReplace(const std::string& search, const std::string& replace, std::string& subject, int* count /* = nullptr */)
{
   if (count != nullptr)
      (*count) = 0;
   if (search.length() == 0)
      return subject;
   size_t start_pos = 0;
   while ((start_pos = subject.find(search, start_pos)) != std::string::npos)
   {
      subject.replace(start_pos, search.length(), replace);
      start_pos += replace.length();
      if (count != nullptr)
         (*count)++;
   }
   return subject;
}
