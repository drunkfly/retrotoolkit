package java.lang;

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;
import java.lang.annotation.ElementType;

@Retention(RetentionPolicy.RUNTIME)
@Target(value={ElementType.CONSTRUCTOR,
               ElementType.FIELD,
               ElementType.LOCAL_VARIABLE,
               ElementType.METHOD,
               ElementType.PACKAGE,
               ElementType.PARAMETER,
               ElementType.TYPE})
public @interface Deprecated
{
}
