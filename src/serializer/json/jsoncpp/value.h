#ifndef CPPTL_JSON_H_INCLUDED
# define CPPTL_JSON_H_INCLUDED

# include "forwards.h"
# include <string>
# include <vector>

# ifndef JSON_USE_CPPTL_SMALLMAP
#  include <map>
# else
#  include <cpptl/smallmap.h>
# endif
# ifdef JSON_USE_CPPTL
#  include <cpptl/forwards.h>
# endif


/** \brief JSON (JavaScript Object Notation).
 */
namespace Json {

   /** \brief Type of the value held by a Value object.
    */
   enum ValueType
   {
      nullValue = 0, ///< 'null' value
      intValue,      ///< signed integer value
      uintValue,     ///< unsigned integer value
      realValue,     ///< double value
      stringValue,   ///< UTF-8 string value
      booleanValue,  ///< bool value
      arrayValue,    ///< array value (ordered list)
      objectValue    ///< object value (collection of name/value pairs).
   };

   enum CommentPlacement
   {
      commentBefore = 0,        ///< a comment placed on the line before a value
      commentAfterOnSameLine,   ///< a comment just after a value on the same line
      commentAfter,             ///< a comment on the line after a value (only make sense for root value)
      numberOfCommentPlacement
   };


   class JSON_API Value 
   {
	   friend class ValueIteratorBase;
# ifdef JSON_VALUE_USE_INTERNAL_MAP
	   friend class ValueInternalLink;
	   friend class ValueInternalMap;
# endif
   public:
	   typedef std::vector<std::string> Members;
	   typedef ValueIterator iterator;
	   typedef ValueConstIterator const_iterator;
	   typedef Json::UInt UInt;
	   typedef Json::Int Int;
	   typedef UInt ArrayIndex;

	   //static const Value null;
	   static const Int minInt = Int( ~(UInt(-1)/2) );
	   static const Int maxInt = Int( UInt(-1)/2 );
	   static const UInt maxUInt = UInt(-1);
   };
} // namespace Json


#endif // CPPTL_JSON_H_INCLUDED
